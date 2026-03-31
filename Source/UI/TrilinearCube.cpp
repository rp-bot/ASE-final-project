#include "TrilinearCube.h"

namespace UI
{
TrilinearCube::TrilinearCube()
{
    openGLContext.setRenderer (this);
    openGLContext.setContinuousRepainting (true);
    openGLContext.attachTo (*this);

    setCursorPosition (0.5f, 0.5f, 0.5f);
}

void TrilinearCube::setCursorPosition (float x, float y, float z)
{
    cursor.x = juce::jlimit (0.0f, 1.0f, x);
    cursor.y = juce::jlimit (0.0f, 1.0f, y);
    cursor.z = juce::jlimit (0.0f, 1.0f, z);
}

void TrilinearCube::mouseDown (const juce::MouseEvent& event)
{
    lastDragPosition = event.getPosition();
}

void TrilinearCube::mouseDrag (const juce::MouseEvent& event)
{
    const auto position = event.getPosition();
    const int dx = position.x - lastDragPosition.x;
    const int dy = position.y - lastDragPosition.y;
    lastDragPosition = position;

    constexpr float sensitivity = 0.5f;
    rotationY += static_cast<float> (dx) * sensitivity;
    rotationX += static_cast<float> (dy) * sensitivity;
    rotationX = juce::jlimit (-89.0f, 89.0f, rotationX);
}

void TrilinearCube::newOpenGLContextCreated()
{
    juce::gl::glEnable (juce::gl::GL_DEPTH_TEST);
    juce::gl::glEnable (juce::gl::GL_BLEND);
    juce::gl::glBlendFunc (juce::gl::GL_SRC_ALPHA, juce::gl::GL_ONE_MINUS_SRC_ALPHA);

    auto shader = std::make_unique<juce::OpenGLShaderProgram> (openGLContext);

    const char* vertexShader = R"(
        varying vec3 vPosition;
        void main()
        {
            vPosition = gl_Vertex.xyz;
            gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        }
    )";

    const char* fragmentShader = R"(
        varying vec3 vPosition;
        uniform float uTime;
        void main()
        {
            vec3 base = vec3 (0.10, 0.45, 1.00);
            float pulse = 0.5 + 0.5 * sin (uTime * 2.0 + dot (vPosition, vec3 (2.0, 2.0, 2.0)));
            float depthGlow = 0.35 + 0.65 * (1.0 - abs (vPosition.z));
            vec3 colour = base * (0.65 + 0.35 * pulse) * depthGlow;
            float alpha = 0.22 + 0.18 * pulse;
            gl_FragColor = vec4 (colour, alpha);
        }
    )";

    if (shader->addVertexShader (juce::OpenGLHelpers::translateVertexShaderToV3 (vertexShader))
        && shader->addFragmentShader (juce::OpenGLHelpers::translateFragmentShaderToV3 (fragmentShader))
        && shader->link())
    {
        timeUniform = std::make_unique<juce::OpenGLShaderProgram::Uniform> (*shader, "uTime");
        shaderProgram = std::move (shader);
    }
    else
    {
        shaderProgram.reset();
        timeUniform.reset();
        jassertfalse;
    }
}

void TrilinearCube::renderOpenGL()
{
    const auto area = getLocalBounds();
    const float scale = static_cast<float> (openGLContext.getRenderingScale());
    const int viewportWidth = juce::roundToInt (scale * static_cast<float> (area.getWidth()));
    const int viewportHeight = juce::roundToInt (scale * static_cast<float> (area.getHeight()));
    const int squareSize = juce::jmin (viewportWidth, viewportHeight);
    const int viewportX = (viewportWidth - squareSize) / 2;
    const int viewportY = (viewportHeight - squareSize) / 2;
    juce::gl::glViewport (viewportX, viewportY, squareSize, squareSize);
    // juce::OpenGLHelpers::clear (juce::Colours::transparentBlack);
    juce::OpenGLHelpers::clear (juce::Colour(0xff323e44));


    juce::gl::glMatrixMode (juce::gl::GL_PROJECTION);
    juce::gl::glLoadIdentity();
    juce::gl::glFrustum (-1.0, 1.0, -1.0, 1.0, 2.0, 10.0);

    juce::gl::glMatrixMode (juce::gl::GL_MODELVIEW);
    juce::gl::glLoadIdentity();
    juce::gl::glTranslatef (0.0f, 0.0f, -4.0f);
    juce::gl::glRotatef (rotationX, 1.0f, 0.0f, 0.0f);
    juce::gl::glRotatef (rotationY, 0.0f, 1.0f, 0.0f);

    if (shaderProgram != nullptr)
    {
        shaderProgram->use();
        if (timeUniform != nullptr)
            timeUniform->set (static_cast<float> (juce::Time::getMillisecondCounterHiRes() * 0.001));
    }

    juce::gl::glBegin (juce::gl::GL_QUADS);
    // Front
    juce::gl::glVertex3f (-1.0f, -1.0f,  1.0f); juce::gl::glVertex3f ( 1.0f, -1.0f,  1.0f);
    juce::gl::glVertex3f ( 1.0f,  1.0f,  1.0f); juce::gl::glVertex3f (-1.0f,  1.0f,  1.0f);
    // Back
    juce::gl::glVertex3f (-1.0f, -1.0f, -1.0f); juce::gl::glVertex3f (-1.0f,  1.0f, -1.0f);
    juce::gl::glVertex3f ( 1.0f,  1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f, -1.0f, -1.0f);
    // Left
    juce::gl::glVertex3f (-1.0f, -1.0f, -1.0f); juce::gl::glVertex3f (-1.0f, -1.0f,  1.0f);
    juce::gl::glVertex3f (-1.0f,  1.0f,  1.0f); juce::gl::glVertex3f (-1.0f,  1.0f, -1.0f);
    // Right
    juce::gl::glVertex3f ( 1.0f, -1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f,  1.0f, -1.0f);
    juce::gl::glVertex3f ( 1.0f,  1.0f,  1.0f); juce::gl::glVertex3f ( 1.0f, -1.0f,  1.0f);
    // Top
    juce::gl::glVertex3f (-1.0f,  1.0f, -1.0f); juce::gl::glVertex3f (-1.0f,  1.0f,  1.0f);
    juce::gl::glVertex3f ( 1.0f,  1.0f,  1.0f); juce::gl::glVertex3f ( 1.0f,  1.0f, -1.0f);
    // Bottom
    juce::gl::glVertex3f (-1.0f, -1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f, -1.0f, -1.0f);
    juce::gl::glVertex3f ( 1.0f, -1.0f,  1.0f); juce::gl::glVertex3f (-1.0f, -1.0f,  1.0f);
    juce::gl::glEnd();

    juce::gl::glUseProgram (0);
    juce::gl::glColor4f (0.8f, 0.9f, 1.0f, 0.9f);
    juce::gl::glLineWidth (1.5f);
    juce::gl::glBegin (juce::gl::GL_LINES);
    // Bottom square
    juce::gl::glVertex3f (-1.0f, -1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f, -1.0f, -1.0f);
    juce::gl::glVertex3f ( 1.0f, -1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f, -1.0f,  1.0f);
    juce::gl::glVertex3f ( 1.0f, -1.0f,  1.0f); juce::gl::glVertex3f (-1.0f, -1.0f,  1.0f);
    juce::gl::glVertex3f (-1.0f, -1.0f,  1.0f); juce::gl::glVertex3f (-1.0f, -1.0f, -1.0f);
    // Top square
    juce::gl::glVertex3f (-1.0f,  1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f,  1.0f, -1.0f);
    juce::gl::glVertex3f ( 1.0f,  1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f,  1.0f,  1.0f);
    juce::gl::glVertex3f ( 1.0f,  1.0f,  1.0f); juce::gl::glVertex3f (-1.0f,  1.0f,  1.0f);
    juce::gl::glVertex3f (-1.0f,  1.0f,  1.0f); juce::gl::glVertex3f (-1.0f,  1.0f, -1.0f);
    // Vertical edges
    juce::gl::glVertex3f (-1.0f, -1.0f, -1.0f); juce::gl::glVertex3f (-1.0f,  1.0f, -1.0f);
    juce::gl::glVertex3f ( 1.0f, -1.0f, -1.0f); juce::gl::glVertex3f ( 1.0f,  1.0f, -1.0f);
    juce::gl::glVertex3f ( 1.0f, -1.0f,  1.0f); juce::gl::glVertex3f ( 1.0f,  1.0f,  1.0f);
    juce::gl::glVertex3f (-1.0f, -1.0f,  1.0f); juce::gl::glVertex3f (-1.0f,  1.0f,  1.0f);
    juce::gl::glEnd();

    const auto map = [] (float v) { return -1.0f + (2.0f * v); };
    const auto px = map (cursor.x);
    const auto py = map (cursor.y);
    const auto pz = map (cursor.z);

    juce::gl::glPointSize (8.0f);
    juce::gl::glColor4f (1.0f, 0.3f, 0.3f, 1.0f);
    juce::gl::glBegin (juce::gl::GL_POINTS);
    juce::gl::glVertex3f (px, py, pz);
    juce::gl::glEnd();
}

void TrilinearCube::openGLContextClosing()
{
    timeUniform.reset();
    shaderProgram.reset();
}
} // namespace UI
