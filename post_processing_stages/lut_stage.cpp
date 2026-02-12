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

#include <epoxy/egl.h>
#include <epoxy/gl.h>

#include "core/post_processor.hpp"

class LutStage : public PostProcessingStage
{
public:
	LutStage(RPiCamApp *app) : PostProcessingStage(app), egl_display_(EGL_NO_DISPLAY), egl_context_(EGL_NO_CONTEXT) {}

    ~LutStage()
    {
        if (egl_display_ != EGL_NO_DISPLAY) {
            eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (egl_context_ != EGL_NO_CONTEXT)
                eglDestroyContext(egl_display_, egl_context_);
            eglTerminate(egl_display_);
        }
    }

	char const *Name() const override { return "lut"; }

	void Read(boost::property_tree::ptree const &params) override
	{
		// TODO: Load LUT path and other parameters
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
        
        // TODO: Load Shaders
	}

	bool Process(CompletedRequestPtr &completed_request) override
	{
        // Ensure context is current (in case thread changed or context was unbound)
        // Optimization: Check if already current?
        // eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context_);

		// TODO: Apply LUT using GPU
		return false; 
	}

	void Stop() override
	{
        // Cleanup happens in destructor or here if explicit stop needed
	}

private:
    EGLDisplay egl_display_;
    EGLContext egl_context_;
};

static PostProcessingStage *Create(RPiCamApp *app)
{
	return new LutStage(app);
}

static RegisterStage reg("lut", &Create);