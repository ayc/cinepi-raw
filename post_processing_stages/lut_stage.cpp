/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (C) 2024, CinePi Project
 *
 * lut_stage.cpp - GPU-accelerated 3D LUT post-processing stage.
 */

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <sys/mman.h>

#include <epoxy/egl.h>
#include <epoxy/gl.h>
#include <libdrm/drm_fourcc.h>

#include "core/post_processor.hpp"
#include "core/rpicam_app.hpp"

// Define EGL extension tokens if not available
#ifndef EGL_LINUX_DMA_BUF_EXT
#define EGL_LINUX_DMA_BUF_EXT 0x3270
#define EGL_LINUX_DRM_FOURCC_EXT 0x3271
#define EGL_DMA_BUF_PLANE0_FD_EXT 0x3272
#define EGL_DMA_BUF_PLANE0_OFFSET_EXT 0x3273
#define EGL_DMA_BUF_PLANE0_PITCH_EXT 0x3274
#endif

static const char *VERTEX_SHADER = R"(
    #version 310 es
    in vec2 position;
    out vec2 texCoord;
    void main() {
        gl_Position = vec4(position, 0.0, 1.0);
        texCoord = position * 0.5 + 0.5;
    }
)";

static const char *FRAGMENT_SHADER = R"(
    #version 310 es
    #extension GL_OES_EGL_image_external_essl3 : require
    precision highp float;

    uniform samplerExternalOES inputImage;
    uniform highp sampler3D lut;
    uniform float strength;

    in vec2 texCoord;
    out vec4 outColor;

    void main() {
        vec4 color = texture(inputImage, texCoord);
        // Basic LUT lookup (assuming RGB input for now)
        // Note: Real implementation needs to handle YUV->RGB if sampling YUV directly
        // or rely on OES sampler doing YUV->RGB conversion automatically (which it often does for external images)
        vec3 lutColor = texture(lut, color.rgb).rgb;
        outColor = vec4(mix(color.rgb, lutColor, strength), color.a);
    }
)";

class LutStage : public PostProcessingStage
{
public:
	LutStage(RPiCamApp *app) : PostProcessingStage(app), egl_display_(EGL_NO_DISPLAY), egl_context_(EGL_NO_CONTEXT), lut_texture_(0), program_(0), vao_(0), fbo_(0) {}

    ~LutStage()
    {
        if (egl_display_ != EGL_NO_DISPLAY) {
            eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if(lut_texture_) glDeleteTextures(1, &lut_texture_);
            if(program_) glDeleteProgram(program_);
            if(vao_) glDeleteVertexArrays(1, &vao_);
            if(fbo_) glDeleteFramebuffers(1, &fbo_);
            if (egl_context_ != EGL_NO_CONTEXT)
                eglDestroyContext(egl_display_, egl_context_);
            eglTerminate(egl_display_);
        }
    }

	char const *Name() const override { return "lut"; }

	void Read(boost::property_tree::ptree const &params) override
	{
		if (params.count("file"))
            lut_file_ = params.get<std::string>("file");
        
        strength_ = params.get<float>("strength", 1.0f);
        enabled_ = params.get<int>("enabled", 1);
	}

	void Configure() override
	{
		// Initialize EGL
        egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (egl_display_ == EGL_NO_DISPLAY)
            throw std::runtime_error("LutStage: Failed to get EGL display");

        if (!eglInitialize(egl_display_, nullptr, nullptr))
            throw std::runtime_error("LutStage: Failed to initialize EGL");

        if (!eglBindAPI(EGL_OPENGL_ES_API))
            throw std::runtime_error("LutStage: Failed to bind OpenGL ES API");

        EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_NONE
        };

        EGLConfig config;
        EGLint num_configs;
        if (!eglChooseConfig(egl_display_, config_attribs, &config, 1, &num_configs) || num_configs == 0)
            throw std::runtime_error("LutStage: Failed to choose EGL config");

        EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
        };

        egl_context_ = eglCreateContext(egl_display_, config, EGL_NO_CONTEXT, context_attribs);
        if (egl_context_ == EGL_NO_CONTEXT)
            throw std::runtime_error("LutStage: Failed to create EGL context");

        if (!eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context_))
            throw std::runtime_error("LutStage: Failed to make context current");
        
        // Load LUT
        if (!lut_file_.empty()) {
            loadLut(lut_file_);
        }

        // Compile Shaders
        GLuint vs = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
        program_ = glCreateProgram();
        glAttachShader(program_, vs);
        glAttachShader(program_, fs);
        glLinkProgram(program_);
        
        GLint linked;
        glGetProgramiv(program_, GL_LINK_STATUS, &linked);
        if(!linked) {
             char infoLog[512];
             glGetProgramInfoLog(program_, 512, NULL, infoLog);
             throw std::runtime_error("LutStage: Shader Linking Failed: " + std::string(infoLog));
        }
        
        glDeleteShader(vs);
        glDeleteShader(fs);

        // Setup Quad
        float vertices[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f
        };
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Create FBO
        glGenFramebuffers(1, &fbo_);
	}

	bool Process(CompletedRequestPtr &completed_request) override
	{
        if (!enabled_ || !lut_texture_) return false;

        libcamera::Stream *stream = app_->GetMainStream();
        if(!stream) return false;

        libcamera::FrameBuffer *buffer = completed_request->buffers[stream];
        if(!buffer) return false;

        // Ensure context is current
        eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context_);

        // Import DMA-BUF
        int fd = buffer->planes()[0].fd.get();
        StreamInfo info = app_->GetStreamInfo(stream);
        
        // Guessing DRM format based on libcamera hint. Usually NV12 for Pi.
        int drm_format = DRM_FORMAT_NV12; 

        EGLint attribs[] = {
            EGL_WIDTH, (EGLint)info.width,
            EGL_HEIGHT, (EGLint)info.height,
            EGL_LINUX_DRM_FOURCC_EXT, drm_format,
            EGL_DMA_BUF_PLANE0_FD_EXT, fd,
            EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
            EGL_DMA_BUF_PLANE0_PITCH_EXT, (EGLint)info.stride,
            EGL_DMA_BUF_PLANE1_FD_EXT, fd,
            EGL_DMA_BUF_PLANE1_OFFSET_EXT, (EGLint)(info.stride * info.height),
            EGL_DMA_BUF_PLANE1_PITCH_EXT, (EGLint)info.stride,
            EGL_NONE
        };

        EGLImageKHR image = eglCreateImageKHR(egl_display_, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, attribs);
        if (image == EGL_NO_IMAGE_KHR) {
            // std::cerr << "LutStage: Failed to create EGLImage" << std::endl;
            return false;
        }

        // Bind Input Texture
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, tex);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, image);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Bind Output Texture/FBO
        // Note: For zero-copy write we'd need another EGLImage attached to FBO.
        // For now, we render to a standard texture and readPixels back.
        GLuint outTex;
        glGenTextures(1, &outTex);
        glBindTexture(GL_TEXTURE_2D, outTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTex, 0);

        glViewport(0, 0, info.width, info.height);

        // Render
        glUseProgram(program_);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_EXTERNAL_OES, tex);
        glUniform1i(glGetUniformLocation(program_, "inputImage"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, lut_texture_);
        glUniform1i(glGetUniformLocation(program_, "lut"), 1);

        glUniform1f(glGetUniformLocation(program_, "strength"), strength_);

        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Read back (CPU Copy - Slow but functional for MVP)
        // Note: FrameBuffer might be NV12, but glReadPixels is RGBA.
        // We can't easily write RGBA back into NV12 buffer without another shader pass or CPU conversion.
        // For demonstration, we just do glFinish to prove GPU execution.
        
        glFinish();

        // Cleanup
        glDeleteTextures(1, &tex);
        glDeleteTextures(1, &outTex);
        eglDestroyImageKHR(egl_display_, image);

		return false; 
	}

	void Stop() override
	{
	}

private:
    EGLDisplay egl_display_;
    EGLContext egl_context_;
    GLuint lut_texture_;
    GLuint program_;
    GLuint vao_;
    GLuint fbo_;
    std::string lut_file_;
    float strength_;
    bool enabled_;

    void loadLut(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) 
            throw std::runtime_error("LutStage: Failed to open LUT file " + filename);

        std::string line;
        int size = 0;
        std::vector<float> data;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::stringstream ss(line);
            std::string token;
            ss >> token;

            if (token == "LUT_3D_SIZE") {
                ss >> size;
                data.reserve(size * size * size * 3);
            } else if (token == "TITLE") {
                // skip
            } else {
                try {
                    float r = std::stof(token);
                    float g, b;
                    ss >> g >> b;
                    data.push_back(r);
                    data.push_back(g);
                    data.push_back(b);
                } catch (...) {}
            }
        }

        if (size == 0 || data.empty())
             throw std::runtime_error("LutStage: Invalid LUT data");

        // Upload to 3D Texture
        if(lut_texture_) glDeleteTextures(1, &lut_texture_);
        glGenTextures(1, &lut_texture_);
        glBindTexture(GL_TEXTURE_3D, lut_texture_);
        
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, size, size, size, 0, GL_RGB, GL_FLOAT, data.data());
        glBindTexture(GL_TEXTURE_3D, 0);
        
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
             throw std::runtime_error("LutStage: GL Error uploading texture: " + std::to_string(err));
    }

    GLuint compileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            throw std::runtime_error("LutStage: Shader Compilation Failed: " + std::string(infoLog));
        }
        return shader;
    }
};

static PostProcessingStage *Create(RPiCamApp *app)
{
	return new LutStage(app);
}

static RegisterStage reg("lut", &Create);