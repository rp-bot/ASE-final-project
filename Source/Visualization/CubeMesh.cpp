#include "CubeMesh.h"

#include <juce_opengl/juce_opengl.h>
#include <glm/gtc/type_ptr.hpp>

namespace Visualization
{
    namespace
    {
        using namespace juce::gl;

        struct CubeVertex
        {
            float x, y, z;
        };

        constexpr unsigned int positionAttribIndex = 0;

        constexpr const char* vertexShaderSource = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}
)";
        constexpr const char* fragmentShaderSource = R"(#version 330 core
out vec4 fragColor;
uniform float uTime;
void main() {
    vec3 base = vec3(0.10, 0.45, 1.00);
    float pulse = 0.5 + 0.5 * sin(uTime * 2.0);
    vec3 colour = base * (0.65 + 0.35 * pulse);
    float alpha = 0.50 + 0.28 * pulse;
    fragColor = vec4(colour, alpha);
}
)";
    }

    void CubeMesh::initialise()
    {
        if (vao_ != 0)
            return;

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
        uTimeUniform_ = glGetUniformLocation(program, "uTime");

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ebo_);

        updateBuffers();
    }

    void CubeMesh::shutdown()
    {
        if (shaderProgram_ != 0)
        {
            glDeleteProgram(shaderProgram_);
            shaderProgram_ = 0;
        }
        if (ebo_ != 0)
        {
            glDeleteBuffers(1, &ebo_);
            ebo_ = 0;
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

    void CubeMesh::setBounds(const glm::vec3& minBounds,
                             const glm::vec3& maxBounds)
    {
        jassert(minBounds.x <= maxBounds.x);
        jassert(minBounds.y <= maxBounds.y);
        jassert(minBounds.z <= maxBounds.z);

        minBounds_ = minBounds;
        maxBounds_ = maxBounds;
        buffersDirty_ = true;
    }

    void CubeMesh::render(const glm::mat4& viewMatrix,
                          const glm::mat4& projectionMatrix,
                          float timeSec,
                          const glm::mat4& modelMatrix) const
    {
        if (vao_ == 0 || shaderProgram_ == 0)
            return;

        if (buffersDirty_)
        {
            const_cast<CubeMesh*>(this)->updateBuffers();
        }

        glUseProgram(shaderProgram_);
        const GLint modelLoc = glGetUniformLocation(shaderProgram_, "uModel");
        const GLint viewLoc = glGetUniformLocation(shaderProgram_, "uView");
        const GLint projLoc = glGetUniformLocation(shaderProgram_, "uProjection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        if (uTimeUniform_ >= 0)
            glUniform1f(uTimeUniform_, timeSec);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(1.5f);

        glBindVertexArray(vao_);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glUseProgram(0);
    }

    void CubeMesh::updateBuffers()
    {
        if (vao_ == 0 || vbo_ == 0 || ebo_ == 0)
            return;

        const CubeVertex vertices[8] = {
            { minBounds_.x, minBounds_.y, minBounds_.z }, // 0
            { maxBounds_.x, minBounds_.y, minBounds_.z }, // 1
            { minBounds_.x, maxBounds_.y, minBounds_.z }, // 2
            { maxBounds_.x, maxBounds_.y, minBounds_.z }, // 3
            { minBounds_.x, minBounds_.y, maxBounds_.z }, // 4
            { maxBounds_.x, minBounds_.y, maxBounds_.z }, // 5
            { minBounds_.x, maxBounds_.y, maxBounds_.z }, // 6
            { maxBounds_.x, maxBounds_.y, maxBounds_.z }, // 7
        };

        const unsigned int indices[24] = {
            // bottom
            0, 1,
            1, 3,
            3, 2,
            2, 0,
            // top
            4, 5,
            5, 7,
            7, 6,
            6, 4,
            // verticals
            0, 4,
            1, 5,
            2, 6,
            3, 7,
        };

        glBindVertexArray(vao_);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(positionAttribIndex);
        glVertexAttribPointer(positionAttribIndex, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), reinterpret_cast<void*>(0));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        buffersDirty_ = false;
    }
}

