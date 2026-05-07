#include "TransformGizmo.h"

#include <juce_opengl/juce_opengl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <vector>

namespace Visualization
{
    using namespace juce::gl;

    namespace
    {
        // ── Arrow geometry ────────────────────────────────────────────────────
        constexpr int   kSlices   = 10;
        constexpr float kShaftR   = 0.04f;  // cylinder radius
        constexpr float kHeadR    = 0.09f;  // cone base radius
        constexpr float kShaftLen = 0.65f;  // shaft occupies y ∈ [0, 0.65]; tip at y=1

        // ── Plane square geometry ─────────────────────────────────────────────
        // Canonical quad in XY plane, local x/y ∈ [kQuadOff, kQuadOff+kQuadSz].
        constexpr float kQuadOff = 0.20f;
        constexpr float kQuadSz  = 0.25f;

        // ── Hit-test capsule radius (fraction of gizmoScale) ─────────────────
        constexpr float kHitFraction = 0.12f;

        // ── Colours ───────────────────────────────────────────────────────────
        const glm::vec4 kColX  { 0.95f, 0.22f, 0.22f, 1.00f }; // red
        const glm::vec4 kColY  { 0.22f, 0.92f, 0.22f, 1.00f }; // green
        const glm::vec4 kColZ  { 0.22f, 0.45f, 0.95f, 1.00f }; // blue
        const glm::vec4 kColXY { 0.22f, 0.45f, 0.95f, 0.55f }; // blue  – Z locked
        const glm::vec4 kColXZ { 0.22f, 0.92f, 0.22f, 0.55f }; // green – Y locked
        const glm::vec4 kColYZ { 0.95f, 0.22f, 0.22f, 0.55f }; // red   – X locked
        const glm::vec4 kColActive { 1.00f, 0.90f, 0.00f, 1.00f }; // yellow highlight

        // ── GLSL ──────────────────────────────────────────────────────────────
        constexpr const char* kGizmoVert = R"(#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uMVP;
void main() { gl_Position = uMVP * vec4(aPos, 1.0); }
)";
        constexpr const char* kGizmoFrag = R"(#version 330 core
out vec4 fragColor;
uniform vec4 uColor;
void main() { fragColor = uColor; }
)";

        GLuint makeShader(const char* vs, const char* fs)
        {
            auto compile = [] (GLenum t, const char* src) {
                GLuint s = glCreateShader(t);
                glShaderSource(s, 1, &src, nullptr);
                glCompileShader(s);
                return s;
            };
            GLuint v = compile(GL_VERTEX_SHADER,   vs);
            GLuint f = compile(GL_FRAGMENT_SHADER, fs);
            GLuint p = glCreateProgram();
            glAttachShader(p, v);
            glAttachShader(p, f);
            glLinkProgram(p);
            glDeleteShader(v);
            glDeleteShader(f);
            return p;
        }
    }

    // ──────────────────────────────────────────────────────────────────────────
    void TransformGizmo::buildArrowGeo()
    {
        std::vector<glm::vec3>   verts;
        std::vector<unsigned int> idx;

        const float pi2 = 2.0f * 3.14159265f;

        // Returns the base index of the new ring of kSlices vertices.
        auto addRing = [&](float y, float r) -> unsigned int {
            unsigned int base = static_cast<unsigned int>(verts.size());
            for (int i = 0; i < kSlices; ++i)
            {
                float theta = static_cast<float>(i) * pi2 / static_cast<float>(kSlices);
                verts.push_back({ r * std::cos(theta), y, r * std::sin(theta) });
            }
            return base;
        };

        unsigned int r0     = addRing(0.0f,      kShaftR); // shaft bottom
        unsigned int r1     = addRing(kShaftLen, kShaftR); // shaft top
        unsigned int r2     = addRing(kShaftLen, kHeadR);  // cone base

        unsigned int cBot = static_cast<unsigned int>(verts.size());
        verts.push_back({ 0.0f, 0.0f, 0.0f });            // bottom cap centre

        unsigned int tip  = static_cast<unsigned int>(verts.size());
        verts.push_back({ 0.0f, 1.0f, 0.0f });            // arrow tip

        // Bottom cap (CCW when viewed from -Y).
        for (int i = 0; i < kSlices; ++i)
        {
            idx.push_back(cBot);
            idx.push_back(r0 + static_cast<unsigned int>((i + 1) % kSlices));
            idx.push_back(r0 + static_cast<unsigned int>(i));
        }

        // Shaft sides.
        for (int i = 0; i < kSlices; ++i)
        {
            unsigned int a = r0 + static_cast<unsigned int>(i);
            unsigned int b = r0 + static_cast<unsigned int>((i + 1) % kSlices);
            unsigned int c = r1 + static_cast<unsigned int>(i);
            unsigned int d = r1 + static_cast<unsigned int>((i + 1) % kSlices);
            idx.push_back(a); idx.push_back(c); idx.push_back(b);
            idx.push_back(b); idx.push_back(c); idx.push_back(d);
        }

        // Cone sides.
        for (int i = 0; i < kSlices; ++i)
        {
            idx.push_back(r2 + static_cast<unsigned int>(i));
            idx.push_back(tip);
            idx.push_back(r2 + static_cast<unsigned int>((i + 1) % kSlices));
        }

        arrowIndexCount_ = static_cast<int>(idx.size());

        glGenVertexArrays(1, &arrowVao_);
        glGenBuffers(1, &arrowVbo_);
        glGenBuffers(1, &arrowEbo_);
        glBindVertexArray(arrowVao_);
        glBindBuffer(GL_ARRAY_BUFFER, arrowVbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(verts.size() * sizeof(glm::vec3)),
                     verts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrowEbo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(idx.size() * sizeof(unsigned int)),
                     idx.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    void TransformGizmo::buildQuadGeo()
    {
        const float lo = kQuadOff;
        const float hi = kQuadOff + kQuadSz;

        // XY-plane quad, z = 0.  Winding: CCW when viewed from +Z.
        const glm::vec3 verts[4] = {
            { lo, lo, 0.0f },
            { hi, lo, 0.0f },
            { hi, hi, 0.0f },
            { lo, hi, 0.0f },
        };
        const unsigned int idx[6] = { 0, 1, 2,  0, 2, 3 };

        glGenVertexArrays(1, &quadVao_);
        glGenBuffers(1, &quadVbo_);
        glGenBuffers(1, &quadEbo_);
        glBindVertexArray(quadVao_);
        glBindBuffer(GL_ARRAY_BUFFER, quadVbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEbo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    // ──────────────────────────────────────────────────────────────────────────
    void TransformGizmo::initialise()
    {
        shader_ = makeShader(kGizmoVert, kGizmoFrag);
        buildArrowGeo();
        buildQuadGeo();
    }

    void TransformGizmo::shutdown()
    {
        if (quadEbo_)  { glDeleteBuffers(1, &quadEbo_);        quadEbo_  = 0; }
        if (quadVbo_)  { glDeleteBuffers(1, &quadVbo_);        quadVbo_  = 0; }
        if (quadVao_)  { glDeleteVertexArrays(1, &quadVao_);   quadVao_  = 0; }
        if (arrowEbo_) { glDeleteBuffers(1, &arrowEbo_);       arrowEbo_ = 0; }
        if (arrowVbo_) { glDeleteBuffers(1, &arrowVbo_);       arrowVbo_ = 0; }
        if (arrowVao_) { glDeleteVertexArrays(1, &arrowVao_);  arrowVao_ = 0; }
        if (shader_)   { glDeleteProgram(shader_);             shader_   = 0; }
    }

    // ──────────────────────────────────────────────────────────────────────────
    void TransformGizmo::drawMesh(unsigned int     vao,
                                   int              indexCount,
                                   const glm::mat4& model,
                                   const glm::vec4& color,
                                   const glm::mat4& view,
                                   const glm::mat4& proj) const noexcept
    {
        const glm::mat4 mvp = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(shader_, "uMVP"),   1, GL_FALSE, glm::value_ptr(mvp));
        glUniform4fv      (glGetUniformLocation(shader_, "uColor"), 1, glm::value_ptr(color));
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }

    void TransformGizmo::render(const glm::vec3& origin,
                                 float            gizmoScale,
                                 const glm::mat4& view,
                                 const glm::mat4& proj,
                                 Part             activePart)
    {
        if (!shader_ || !arrowVao_ || !quadVao_)
            return;

        // Gizmo is always drawn on top so the user can always interact with it.
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(shader_);

        const glm::mat4 T = glm::translate(glm::mat4(1.f), origin);
        const glm::mat4 S = glm::scale(glm::mat4(1.f), glm::vec3(gizmoScale));

        // Arrow model matrices: T * Raxis * S
        // Canonical arrow points along +Y; rotate to each world axis.
        const glm::mat4 mX = T * glm::rotate(glm::mat4(1.f), -glm::half_pi<float>(), glm::vec3(0.f, 0.f, 1.f)) * S;
        const glm::mat4 mY = T * S;
        const glm::mat4 mZ = T * glm::rotate(glm::mat4(1.f),  glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)) * S;

        // Plane quad model matrices: T * Rplane * S
        // Canonical quad is in XY plane; rotate to each locked-axis plane.
        const glm::mat4 mXY = T * S;  // stays in XY
        const glm::mat4 mXZ = T * glm::rotate(glm::mat4(1.f),  glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)) * S;
        const glm::mat4 mYZ = T * glm::rotate(glm::mat4(1.f), -glm::half_pi<float>(), glm::vec3(0.f, 1.f, 0.f)) * S;

        auto col = [&](Part p, const glm::vec4& base) -> const glm::vec4& {
            return (activePart == p) ? kColActive : base;
        };

        // Plane squares first (behind arrows within the depth-disabled layer).
        drawMesh(quadVao_, 6, mXY, col(Part::PlaneXY, kColXY), view, proj);
        drawMesh(quadVao_, 6, mXZ, col(Part::PlaneXZ, kColXZ), view, proj);
        drawMesh(quadVao_, 6, mYZ, col(Part::PlaneYZ, kColYZ), view, proj);

        // Arrows on top.
        drawMesh(arrowVao_, arrowIndexCount_, mX, col(Part::AxisX, kColX), view, proj);
        drawMesh(arrowVao_, arrowIndexCount_, mY, col(Part::AxisY, kColY), view, proj);
        drawMesh(arrowVao_, arrowIndexCount_, mZ, col(Part::AxisZ, kColZ), view, proj);

        glBindVertexArray(0);
        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    // ──────────────────────────────────────────────────────────────────────────
    bool TransformGizmo::hitCapsule(const Ray&       ray,
                                     const glm::vec3& p0,
                                     const glm::vec3& p1,
                                     float            radius) noexcept
    {
        // Closest distance between the ray and the line segment [p0, p1].
        const glm::vec3 segDir = p1 - p0;
        const glm::vec3 m      = ray.origin - p0;

        const float a = glm::dot(segDir, segDir);         // |seg|^2
        const float e = glm::dot(ray.direction, segDir);  // dot(D, seg)
        const float f = glm::dot(ray.direction, m);       // dot(D, O-p0)
        const float c = glm::dot(segDir, m);              // dot(seg, O-p0)

        const float denom = a - e * e;

        float s, t;
        if (denom < 1e-8f)
        {
            // Ray nearly parallel to segment; project p0 onto ray.
            s = 0.0f;
            t = f;
        }
        else
        {
            s = glm::clamp((c - e * f) / denom, 0.0f, 1.0f);
            t = (c * e - a * f) / denom;
        }

        // Ray can only go forward.
        if (t < 0.0f)
        {
            t = 0.0f;
            s = glm::clamp(c / (a > 1e-8f ? a : 1.0f), 0.0f, 1.0f);
        }

        const glm::vec3 rayPt = ray.origin + t * ray.direction;
        const glm::vec3 segPt = p0 + s * segDir;
        return glm::length(rayPt - segPt) < radius;
    }

    bool TransformGizmo::hitRect(const Ray&       ray,
                                  const glm::vec3& v0,
                                  const glm::vec3& v1,
                                  const glm::vec3& v2,
                                  const glm::vec3& v3) noexcept
    {
        // Intersect ray with the plane of the quad, then check containment.
        const glm::vec3 n = glm::normalize(glm::cross(v1 - v0, v3 - v0));
        const float denom = glm::dot(ray.direction, n);
        if (std::abs(denom) < 1e-6f)
            return false;

        const float t = glm::dot(v0 - ray.origin, n) / denom;
        if (t < 0.0f)
            return false;

        const glm::vec3 p = ray.origin + t * ray.direction;

        // Point-in-quad via two triangle tests.
        auto inTri = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
            const glm::vec3 n1 = glm::cross(b - a, p - a);
            const glm::vec3 n2 = glm::cross(c - b, p - b);
            const glm::vec3 n3 = glm::cross(a - c, p - c);
            return glm::dot(n1, n2) >= 0.0f && glm::dot(n1, n3) >= 0.0f;
        };
        return inTri(v0, v1, v2) || inTri(v0, v2, v3);
    }

    TransformGizmo::Part TransformGizmo::hitTest(const Ray&       ray,
                                                  const glm::vec3& origin,
                                                  float            gizmoScale) const noexcept
    {
        const float r = kHitFraction * gizmoScale;

        // Arrow endpoint tips in world space.
        const glm::vec3 tipX = origin + glm::vec3(gizmoScale, 0.0f,       0.0f);
        const glm::vec3 tipY = origin + glm::vec3(0.0f,       gizmoScale, 0.0f);
        const glm::vec3 tipZ = origin + glm::vec3(0.0f,       0.0f,       gizmoScale);

        // Test arrows first (higher priority than plane squares).
        if (hitCapsule(ray, origin, tipX, r)) return Part::AxisX;
        if (hitCapsule(ray, origin, tipY, r)) return Part::AxisY;
        if (hitCapsule(ray, origin, tipZ, r)) return Part::AxisZ;

        // Plane quad world-space corners (matching the canonical quad after rotation).
        const float lo = kQuadOff * gizmoScale;
        const float hi = (kQuadOff + kQuadSz) * gizmoScale;

        // XY plane (Z locked).
        {
            const glm::vec3 q0 = origin + glm::vec3(lo, lo, 0.f);
            const glm::vec3 q1 = origin + glm::vec3(hi, lo, 0.f);
            const glm::vec3 q2 = origin + glm::vec3(hi, hi, 0.f);
            const glm::vec3 q3 = origin + glm::vec3(lo, hi, 0.f);
            if (hitRect(ray, q0, q1, q2, q3)) return Part::PlaneXY;
        }

        // XZ plane (Y locked).
        {
            const glm::vec3 q0 = origin + glm::vec3(lo, 0.f, lo);
            const glm::vec3 q1 = origin + glm::vec3(hi, 0.f, lo);
            const glm::vec3 q2 = origin + glm::vec3(hi, 0.f, hi);
            const glm::vec3 q3 = origin + glm::vec3(lo, 0.f, hi);
            if (hitRect(ray, q0, q1, q2, q3)) return Part::PlaneXZ;
        }

        // YZ plane (X locked).
        {
            const glm::vec3 q0 = origin + glm::vec3(0.f, lo, lo);
            const glm::vec3 q1 = origin + glm::vec3(0.f, hi, lo);
            const glm::vec3 q2 = origin + glm::vec3(0.f, hi, hi);
            const glm::vec3 q3 = origin + glm::vec3(0.f, lo, hi);
            if (hitRect(ray, q0, q1, q2, q3)) return Part::PlaneYZ;
        }

        return Part::None;
    }
}
