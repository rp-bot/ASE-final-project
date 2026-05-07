#include "GroundGrid.h"

#include <juce_opengl/juce_opengl.h>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

namespace Visualization
{
    using namespace juce::gl;

    namespace
    {
        struct GridVertex
        {
            glm::vec3 position;
            glm::vec4 colour;
        };

        constexpr const char* gridVertSource = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColour;

uniform mat4 uView;
uniform mat4 uProjection;

out vec4 vColour;

void main()
{
    vColour = aColour;
    gl_Position = uProjection * uView * vec4(aPosition, 1.0);
}
)";

        constexpr const char* gridFragSource = R"(#version 330 core
in vec4 vColour;
out vec4 fragColor;

void main()
{
    fragColor = vColour;
}
)";

        GLuint compileShader(GLenum type, const char* src)
        {
            const GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            return s;
        }
    }

    void GroundGrid::setParams(float radius, float spacing, int majorLineEvery)
    {
        radius_ = std::max(0.1f, radius);
        spacing_ = std::max(0.01f, spacing);
        majorLineEvery_ = std::max(1, majorLineEvery);

        lastGridY_ = std::numeric_limits<float>::quiet_NaN();
        lastCenterXZ_ = { std::numeric_limits<float>::quiet_NaN(),
                          std::numeric_limits<float>::quiet_NaN() };
    }

    void GroundGrid::initialise()
    {
        if (vao_ != 0)
            return;

        const GLuint v = compileShader(GL_VERTEX_SHADER, gridVertSource);
        const GLuint f = compileShader(GL_FRAGMENT_SHADER, gridFragSource);
        shaderProgram_ = glCreateProgram();
        glAttachShader(shaderProgram_, v);
        glAttachShader(shaderProgram_, f);
        glLinkProgram(shaderProgram_);
        glDeleteShader(v);
        glDeleteShader(f);

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
    }

    void GroundGrid::shutdown()
    {
        if (shaderProgram_) { glDeleteProgram(shaderProgram_); shaderProgram_ = 0; }
        if (vbo_) { glDeleteBuffers(1, &vbo_); vbo_ = 0; }
        if (vao_) { glDeleteVertexArrays(1, &vao_); vao_ = 0; }
        vertexCount_ = 0;
    }

    void GroundGrid::rebuildGeometry(float gridY, const glm::vec3& centerXZ)
    {
        if (vao_ == 0 || vbo_ == 0)
            return;

        std::vector<GridVertex> verts;
        verts.reserve(4096);

        const float cx = centerXZ.x;
        const float cz = centerXZ.z;

        const int steps = static_cast<int>(std::floor(radius_ / spacing_));
        const float extent = static_cast<float>(steps) * spacing_;

        // Designed for a light background (see Renderer3D clear colour).
        const glm::vec4 minor{ 0.0f, 0.0f, 0.0f, 0.08f };
        const glm::vec4 major{ 0.0f, 0.0f, 0.0f, 0.16f };
        const glm::vec4 axisX{ 0.85f, 0.15f, 0.15f, 0.30f };
        const glm::vec4 axisZ{ 0.15f, 0.30f, 0.85f, 0.30f };

        for (int i = -steps; i <= steps; ++i)
        {
            const float t = static_cast<float>(i) * spacing_;
            const bool isMajor = (i % majorLineEvery_) == 0;
            const glm::vec4 col = (i == 0) ? axisZ : (isMajor ? major : minor);

            // Lines parallel to X (vary X, fixed Z)
            verts.push_back({ { cx - extent, gridY, cz + t }, col });
            verts.push_back({ { cx + extent, gridY, cz + t }, col });
        }

        for (int i = -steps; i <= steps; ++i)
        {
            const float t = static_cast<float>(i) * spacing_;
            const bool isMajor = (i % majorLineEvery_) == 0;
            const glm::vec4 col = (i == 0) ? axisX : (isMajor ? major : minor);

            // Lines parallel to Z (vary Z, fixed X)
            verts.push_back({ { cx + t, gridY, cz - extent }, col });
            verts.push_back({ { cx + t, gridY, cz + extent }, col });
        }

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(verts.size() * sizeof(GridVertex)),
                     verts.data(),
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex),
                              reinterpret_cast<void*>(offsetof(GridVertex, position)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GridVertex),
                              reinterpret_cast<void*>(offsetof(GridVertex, colour)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        vertexCount_ = static_cast<int>(verts.size());

        lastGridY_ = gridY;
        lastCenterXZ_ = { cx, cz };
    }

    void GroundGrid::render(const glm::mat4& viewMatrix,
                            const glm::mat4& projectionMatrix,
                            float gridY,
                            const glm::vec3& centerXZ) const
    {
        if (shaderProgram_ == 0 || vao_ == 0 || vbo_ == 0)
            return;

        const auto changed = [&]()
        {
            constexpr float eps = 1.0e-6f;
            if (vertexCount_ == 0)
                return true;
            if (std::abs(gridY - lastGridY_) > eps)
                return true;
            if (std::abs(centerXZ.x - lastCenterXZ_.x) > eps)
                return true;
            if (std::abs(centerXZ.z - lastCenterXZ_.y) > eps)
                return true;
            return false;
        }();

        if (changed)
        {
            const_cast<GroundGrid*>(this)->rebuildGeometry(gridY, centerXZ);
        }

        if (vertexCount_ <= 0)
            return;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(shaderProgram_);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "uView"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, "uProjection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(vao_);
        glDrawArrays(GL_LINES, 0, vertexCount_);
        glBindVertexArray(0);
        glUseProgram(0);

        glDisable(GL_BLEND);
    }
}

