#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>

namespace Visualization
{
    class Renderer3D;

    class GLContextHost : private juce::OpenGLRenderer
    {
    public:
        GLContextHost();
        ~GLContextHost() override;

        void attachTo(juce::Component& component);
        void detach();

        void setRenderer(Renderer3D* renderer);

        void newOpenGLContextCreated() override;
        void renderOpenGL() override;
        void openGLContextClosing() override;

    private:
        juce::OpenGLContext openGLContext_;
        Renderer3D* renderer_{ nullptr };
        juce::Component* attachedComponent_{ nullptr };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GLContextHost)
    };
}

