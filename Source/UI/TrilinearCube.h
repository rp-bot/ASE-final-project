#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_opengl/juce_opengl.h>

namespace UI {

class TrilinearCube : public juce::Component,
                           private juce::OpenGLRenderer {
public:
    TrilinearCube();

    ~TrilinearCube() noexcept override {
        openGLContext.detach();
    }

    // data input/output
    void setCursorPosition (float x, float y, float z);

    std::function<void(float,float,float)> onCursorChanged;

private:
    // OpenGL lifecycle
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    juce::OpenGLContext openGLContext;
    juce::Vector3D<float> cursor;
};

}