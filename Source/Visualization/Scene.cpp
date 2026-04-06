#include "Scene.h"

#include <juce_opengl/juce_opengl.h>
#include <glm/gtc/type_ptr.hpp>

namespace Visualization
{
    using namespace juce::gl;
    namespace
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 colour;
        };

        constexpr const char* vertexShaderSource = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColour;

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vColour;

void main()
{
    vColour = aColour;
    gl_Position = uProjection * uView * vec4(aPosition, 1.0);
}
)";

        constexpr const char* fragmentShaderSource = R"(#version 330 core
in vec3 vColour;
out vec4 fragColor;

void main()
{
    fragColor = vec4(vColour, 1.0);
}
)";
    }

    void Scene::initialise()
    {
        const Vertex vertices[]
        {
            // X axis (red)
            { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            // Y axis (green)
            { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            // Z axis (blue)
            { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
            { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        };

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, position)));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, colour)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        const auto compileShader = [] (GLenum type, const char* source) -> GLuint
        {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);
            return shader;
        };

        const GLuint vert = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
        const GLuint frag = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

        GLuint program = glCreateProgram();
        glAttachShader(program, vert);
        glAttachShader(program, frag);
        glLinkProgram(program);

        glDeleteShader(vert);
        glDeleteShader(frag);

        shaderProgram_ = program;
    }

    void Scene::shutdown()
    {
        if (shaderProgram_ != 0)
        {
            glDeleteProgram(shaderProgram_);
            shaderProgram_ = 0;
        }

        if (vbo_ != 0)
        {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }

        if (vao_ != 0)
        {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0;
        }
    }

    void Scene::render(const glm::mat4& viewMatrix,
                       const glm::mat4& projectionMatrix)
    {
        if (shaderProgram_ == 0 || vao_ == 0)
            return;

        glUseProgram(shaderProgram_);

        const GLint viewLoc = glGetUniformLocation(shaderProgram_, "uView");
        const GLint projLoc = glGetUniformLocation(shaderProgram_, "uProjection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);

        glUseProgram(0);
    }
}

