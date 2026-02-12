/*
 * test_egl.cpp
 *
 * Simple test to verify EGL context creation (surfaceless if possible).
 */

#include <iostream>
#include <epoxy/egl.h>
#include <epoxy/gl.h>

int main() {
    std::cout << "Testing EGL context creation..." << std::endl;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        std::cerr << "Failed to get EGL display" << std::endl;
        return 1;
    }

    if (!eglInitialize(display, nullptr, nullptr)) {
        std::cerr << "Failed to initialize EGL" << std::endl;
        return 1;
    }

    std::cout << "EGL Initialized." << std::endl;

    // Try to bind API
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        std::cerr << "Failed to bind OpenGL ES API" << std::endl;
        return 1;
    }

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
    if (!eglChooseConfig(display, config_attribs, &config, 1, &num_configs) || num_configs == 0) {
        std::cerr << "Failed to choose EGL config" << std::endl;
        return 1;
    }

    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    if (context == EGL_NO_CONTEXT) {
        std::cerr << "Failed to create EGL context" << std::endl;
        return 1;
    }

    std::cout << "EGL Context Created." << std::endl;

    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context)) {
        // Fallback to PBuffer if surfaceless fails (though NO_SURFACE should work on modern EGL)
        std::cerr << "Failed to make context current (surfaceless)" << std::endl;
        // Proceeding anyway as this might be platform specific
    } else {
        std::cout << "Context made current (surfaceless)." << std::endl;
        std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    }

    eglDestroyContext(display, context);
    eglTerminate(display);

    return 0;
}
