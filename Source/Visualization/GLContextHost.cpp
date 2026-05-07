#include "GLContextHost.h"
#include "Renderer3D.h"

namespace Visualization
{
    GLContextHost::GLContextHost() = default;

    GLContextHost::~GLContextHost() = default;

    void GLContextHost::attachTo(juce::Component& component)
    {
        attachedComponent_ = &component;

#if JUCE_MAC
        // macOS can fall back to an older profile unless we explicitly request core OpenGL.
        // The renderer shaders target GLSL 330, which requires a modern context.
        openGLContext_.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL4_1);
#else
        openGLContext_.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL3_2);
#endif

        openGLContext_.setRenderer(this);
        openGLContext_.setContinuousRepainting(true);
        openGLContext_.attachTo(component);
    }

    void GLContextHost::detach()
    {
        openGLContext_.setRenderer(nullptr);
        openGLContext_.detach();
        attachedComponent_ = nullptr;
    }

    void GLContextHost::setRenderer(Renderer3D* renderer)
    {
        renderer_ = renderer;
    }

    void GLContextHost::newOpenGLContextCreated()
    {
        jassert(renderer_ != nullptr);

        if (renderer_ != nullptr)
            renderer_->initialise();
    }

    void GLContextHost::renderOpenGL()
    {
        if (renderer_ == nullptr || attachedComponent_ == nullptr)
            return;

        const auto bounds = attachedComponent_->getLocalBounds();
        const auto scale = static_cast<float>(openGLContext_.getRenderingScale());
        const int width = juce::roundToInt(static_cast<float>(bounds.getWidth()) * scale);
        const int height = juce::roundToInt(static_cast<float>(bounds.getHeight()) * scale);

        if (width <= 0 || height <= 0)
            return;

        renderer_->render(width, height);
    }

    void GLContextHost::openGLContextClosing()
    {
        if (renderer_ != nullptr)
            renderer_->shutdown();
    }
}

