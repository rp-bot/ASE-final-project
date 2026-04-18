#include "Renderer3D.h"

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace Visualization
{
    using namespace juce::gl;

    namespace
    {
        // Light neutral background so future shadow work reads well.
        const juce::Colour sceneClearColour = juce::Colour::fromRGB(236, 238, 242);

        constexpr const char* cursorVertSource = R"(#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uModel;
void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";
        constexpr const char* cursorFragSource = R"(#version 330 core
out vec4 fragColor;
uniform vec4 uColor;
void main() {
    fragColor = uColor;
}
)";

        constexpr float cursorSphereRadius = 0.12f;  // world-space size (cube is 2 units)
        constexpr float cornerSphereRadius = 0.075f; // slightly smaller than cursor marker
        constexpr int sphereSlices = 16;
        constexpr int sphereStacks = 12;

        // Distinct cursor colour (not in the 8 module colours).
        const glm::vec4 cursorColour{ 0.71f, 1.0f, 0.12f, 1.0f }; // chartreuse

        constexpr int shadowMapSize = 2048;

        constexpr const char* shadowDepthVertSource = R"(#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uModel;
uniform mat4 uLightSpace;
void main()
{
    gl_Position = uLightSpace * uModel * vec4(aPos, 1.0);
}
)";
        constexpr const char* shadowDepthFragSource = R"(#version 330 core
void main() { }
)";

        constexpr const char* litVertSource = R"(#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpace;

out vec3 vWorldPos;
out vec3 vNormal;
out vec4 vPosLightSpace;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    vPosLightSpace = uLightSpace * worldPos;
    gl_Position = uProjection * uView * worldPos;
}
)";

        constexpr const char* litFragSource = R"(#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec4 vPosLightSpace;

uniform vec3 uLightDir;   // direction *towards* the scene (world space)
uniform vec3 uViewPos;
uniform vec3 uBaseColor;
uniform float uAlpha;
uniform float uShadowStrength;
uniform sampler2D uShadowMap;
uniform int uUseBlobShadow;
uniform vec3 uCursorWorldPos;
uniform float uPlaneY;
uniform float uBlobRadiusBase;
uniform float uBlobRadiusPerHeight;
uniform float uBlobSoftness;
uniform float uBlobStrength;
uniform int uCubePulseMode;
uniform float uTime;

out vec4 fragColor;

float shadowFactor(vec4 posLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = posLightSpace.xyz / posLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Outside the shadow map -> no shadow.
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.0015 * (1.0 - dot(normalize(normal), normalize(-lightDir))), 0.0006);

    vec2 texelSize = 1.0 / vec2(textureSize(uShadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float closestDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

float blobShadow(vec3 worldPos)
{
    // Project cursor position onto the plane along the light direction.
    float denom = uLightDir.y;
    if (abs(denom) < 1e-5)
        return 0.0;

    float t = (uCursorWorldPos.y - uPlaneY) / denom;
    vec3 projected = uCursorWorldPos - uLightDir * t;

    float h = max(uCursorWorldPos.y - uPlaneY, 0.0);
    float radius = uBlobRadiusBase + uBlobRadiusPerHeight * 0.4 * h;

    float d = length(worldPos.xz - projected.xz);
    float edge0 = max(radius - uBlobSoftness, 0.0);
    float edge1 = radius + uBlobSoftness;

    // 1 in center -> 0 outside, with soft edge.
    float core = 1.0 - smoothstep(edge0, edge1, d);

    // Fade with height (further away -> softer + lighter).
    float strength = uBlobStrength / (1.0 + 0.6 * h);
    return clamp(core * strength, 0.0, 1.0);
}

void main()
{
    if (uCubePulseMode != 0)
    {
        vec3 base = vec3(0.10, 0.45, 1.00);
        float pulse = 0.5 + 0.5 * sin(uTime * 2.0 + dot(vWorldPos, vec3(2.0)));
        vec3 colour = base * (0.65 + 0.35 * pulse);
        float alpha = 0.22 + 0.18 * pulse;
        fragColor = vec4(colour, alpha);
        return;
    }

    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir);
    float diff = max(dot(N, L), 0.0);

    float shadow = 0.0;
    if (uUseBlobShadow != 0)
        shadow = blobShadow(vWorldPos);
    else
        shadow = shadowFactor(vPosLightSpace, N, uLightDir) * clamp(uShadowStrength, 0.0, 1.0);

    float ambient = 0.38;
    vec3 lit = uBaseColor * (ambient + (1.0 - shadow) * diff);
    fragColor = vec4(lit, uAlpha);
}
)";

        GLuint compileShader(GLenum type, const char* src)
        {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            return s;
        }

        GLuint linkProgram(const char* vs, const char* fs)
        {
            const GLuint v = compileShader(GL_VERTEX_SHADER, vs);
            const GLuint f = compileShader(GL_FRAGMENT_SHADER, fs);
            const GLuint p = glCreateProgram();
            glAttachShader(p, v);
            glAttachShader(p, f);
            glLinkProgram(p);
            glDeleteShader(v);
            glDeleteShader(f);
            return p;
        }

        struct PosNormal
        {
            glm::vec3 pos;
            glm::vec3 normal;
        };

        void buildUnitSphere(std::vector<glm::vec3>& outVertices, std::vector<unsigned int>& outIndices)
        {
            outVertices.clear();
            outIndices.clear();
            for (int stack = 0; stack <= sphereStacks; ++stack)
            {
                const float phi = static_cast<float>(stack) * static_cast<float>(3.14159265f) / static_cast<float>(sphereStacks);
                const float y = std::cos(phi);
                const float r = std::sin(phi);
                for (int slice = 0; slice <= sphereSlices; ++slice)
                {
                    const float theta = static_cast<float>(slice) * 2.0f * static_cast<float>(3.14159265f) / static_cast<float>(sphereSlices);
                    outVertices.push_back(glm::vec3(r * std::cos(theta), y, r * std::sin(theta)));
                }
            }
            for (int stack = 0; stack < sphereStacks; ++stack)
            {
                for (int slice = 0; slice < sphereSlices; ++slice)
                {
                    const int a = stack * (sphereSlices + 1) + slice;
                    const int b = a + sphereSlices + 1;
                    outIndices.push_back(static_cast<unsigned int>(a));
                    outIndices.push_back(static_cast<unsigned int>(b));
                    outIndices.push_back(static_cast<unsigned int>(a + 1));
                    outIndices.push_back(static_cast<unsigned int>(a + 1));
                    outIndices.push_back(static_cast<unsigned int>(b));
                    outIndices.push_back(static_cast<unsigned int>(b + 1));
                }
            }
        }
    }

    void Renderer3D::initialise()
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        juce::OpenGLHelpers::clear(sceneClearColour);

        scene_.initialise();
        cubeMesh_.initialise();
        groundGrid_.initialise();
        groundGrid_.setParams(/*radius*/ 10.0f, /*spacing*/ 0.5f, /*majorLineEvery*/ 4);

        // Shadow shaders + lit shader.
        shadowDepthShader_ = linkProgram(shadowDepthVertSource, shadowDepthFragSource);
        litShader_ = linkProgram(litVertSource, litFragSource);

        // Shadow map FBO + depth texture.
        glGenFramebuffers(1, &shadowFbo_);
        glGenTextures(1, &shadowDepthTex_);
        glBindTexture(GL_TEXTURE_2D, shadowDepthTex_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     shadowMapSize, shadowMapSize, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        const float border[4] = { 1.f, 1.f, 1.f, 1.f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTex_, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Solid cube geometry in local [-1,1] space (positions + normals).
        {
            const PosNormal cubeVerts[] = {
                // +X
                { { 1, -1, -1 }, { 1, 0, 0 } }, { { 1, -1,  1 }, { 1, 0, 0 } }, { { 1,  1,  1 }, { 1, 0, 0 } },
                { { 1, -1, -1 }, { 1, 0, 0 } }, { { 1,  1,  1 }, { 1, 0, 0 } }, { { 1,  1, -1 }, { 1, 0, 0 } },
                // -X
                { { -1, -1,  1 }, { -1, 0, 0 } }, { { -1, -1, -1 }, { -1, 0, 0 } }, { { -1,  1, -1 }, { -1, 0, 0 } },
                { { -1, -1,  1 }, { -1, 0, 0 } }, { { -1,  1, -1 }, { -1, 0, 0 } }, { { -1,  1,  1 }, { -1, 0, 0 } },
                // +Y
                { { -1,  1, -1 }, { 0, 1, 0 } }, { {  1,  1, -1 }, { 0, 1, 0 } }, { {  1,  1,  1 }, { 0, 1, 0 } },
                { { -1,  1, -1 }, { 0, 1, 0 } }, { {  1,  1,  1 }, { 0, 1, 0 } }, { { -1,  1,  1 }, { 0, 1, 0 } },
                // -Y
                { { -1, -1,  1 }, { 0, -1, 0 } }, { {  1, -1,  1 }, { 0, -1, 0 } }, { {  1, -1, -1 }, { 0, -1, 0 } },
                { { -1, -1,  1 }, { 0, -1, 0 } }, { {  1, -1, -1 }, { 0, -1, 0 } }, { { -1, -1, -1 }, { 0, -1, 0 } },
                // +Z
                { {  1, -1, 1 }, { 0, 0, 1 } }, { { -1, -1, 1 }, { 0, 0, 1 } }, { { -1,  1, 1 }, { 0, 0, 1 } },
                { {  1, -1, 1 }, { 0, 0, 1 } }, { { -1,  1, 1 }, { 0, 0, 1 } }, { {  1,  1, 1 }, { 0, 0, 1 } },
                // -Z
                { { -1, -1, -1 }, { 0, 0, -1 } }, { {  1, -1, -1 }, { 0, 0, -1 } }, { {  1,  1, -1 }, { 0, 0, -1 } },
                { { -1, -1, -1 }, { 0, 0, -1 } }, { {  1,  1, -1 }, { 0, 0, -1 } }, { { -1,  1, -1 }, { 0, 0, -1 } },
            };

            glGenVertexArrays(1, &solidCubeVao_);
            glGenBuffers(1, &solidCubeVbo_);
            glBindVertexArray(solidCubeVao_);
            glBindBuffer(GL_ARRAY_BUFFER, solidCubeVbo_);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(cubeVerts)), cubeVerts, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PosNormal), reinterpret_cast<void*>(offsetof(PosNormal, pos)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PosNormal), reinterpret_cast<void*>(offsetof(PosNormal, normal)));
            glBindVertexArray(0);
        }

        // Solid plane (XZ) centered at origin, y=0.
        {
            const PosNormal planeVerts[] = {
                { { -1, 0, -1 }, { 0, 1, 0 } }, { {  1, 0, -1 }, { 0, 1, 0 } }, { {  1, 0,  1 }, { 0, 1, 0 } },
                { { -1, 0, -1 }, { 0, 1, 0 } }, { {  1, 0,  1 }, { 0, 1, 0 } }, { { -1, 0,  1 }, { 0, 1, 0 } },
            };
            glGenVertexArrays(1, &solidPlaneVao_);
            glGenBuffers(1, &solidPlaneVbo_);
            glBindVertexArray(solidPlaneVao_);
            glBindBuffer(GL_ARRAY_BUFFER, solidPlaneVbo_);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(planeVerts)), planeVerts, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PosNormal), reinterpret_cast<void*>(offsetof(PosNormal, pos)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PosNormal), reinterpret_cast<void*>(offsetof(PosNormal, normal)));
            glBindVertexArray(0);
        }

        const auto compile = [] (GLenum type, const char* src) {
            GLuint s = glCreateShader(type);
            glShaderSource(s, 1, &src, nullptr);
            glCompileShader(s);
            return s;
        };
        GLuint v = compile(GL_VERTEX_SHADER, cursorVertSource);
        GLuint f = compile(GL_FRAGMENT_SHADER, cursorFragSource);
        cursorShader_ = glCreateProgram();
        glAttachShader(cursorShader_, v);
        glAttachShader(cursorShader_, f);
        glLinkProgram(cursorShader_);
        glDeleteShader(v);
        glDeleteShader(f);

        std::vector<glm::vec3> sphereVerts;
        std::vector<unsigned int> sphereIndices;
        buildUnitSphere(sphereVerts, sphereIndices);
        cursorSphereIndexCount_ = static_cast<int>(sphereIndices.size());

        glGenVertexArrays(1, &cursorSphereVao_);
        glGenBuffers(1, &cursorSphereVbo_);
        glGenBuffers(1, &cursorSphereEbo_);
        glBindVertexArray(cursorSphereVao_);
        glBindBuffer(GL_ARRAY_BUFFER, cursorSphereVbo_);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphereVerts.size() * sizeof(glm::vec3)), sphereVerts.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cursorSphereEbo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphereIndices.size() * sizeof(unsigned int)), sphereIndices.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);

        rayCaster_.setCamera(&camera_);

        // Default colours matching the editor modules (in case the editor doesn't set them).
        cornerColours_ = {
            glm::vec4(231.0f / 255.0f,  76.0f / 255.0f,  60.0f / 255.0f, 1.0f),
            glm::vec4(230.0f / 255.0f, 126.0f / 255.0f,  34.0f / 255.0f, 1.0f),
            glm::vec4(241.0f / 255.0f, 196.0f / 255.0f,  15.0f / 255.0f, 1.0f),
            glm::vec4( 46.0f / 255.0f, 204.0f / 255.0f, 113.0f / 255.0f, 1.0f),
            glm::vec4( 26.0f / 255.0f, 188.0f / 255.0f, 156.0f / 255.0f, 1.0f),
            glm::vec4( 52.0f / 255.0f, 152.0f / 255.0f, 219.0f / 255.0f, 1.0f),
            glm::vec4(155.0f / 255.0f,  89.0f / 255.0f, 182.0f / 255.0f, 1.0f),
            glm::vec4(236.0f / 255.0f, 112.0f / 255.0f, 173.0f / 255.0f, 1.0f),
        };
    }

    namespace
    {
        /** Ray vs plane (planePoint, planeNormal); ray direction must be unit. */
        bool intersectRayPlane(const Ray& ray, const glm::vec3& planePoint,
                               const glm::vec3& planeNormal, glm::vec3& outPoint)
        {
            const float denom = glm::dot(ray.direction, planeNormal);
            if (std::abs(denom) < 1e-6f)
                return false;
            const float t = glm::dot(planePoint - ray.origin, planeNormal) / denom;
            if (t < 0.0f)
                return false;
            outPoint = ray.origin + t * ray.direction;
            return true;
        }

        /** planeNormal must be unit. Removes drift along the view axis after AABB clamp. */
        void projectOntoPlane(glm::vec3& p, const glm::vec3& planePoint, const glm::vec3& planeNormal)
        {
            p -= glm::dot(p - planePoint, planeNormal) * planeNormal;
        }

        /** Keep cursor inside the cube while staying on the slide plane (orthogonal to camera). */
        void clampCursorToCubeOnViewPlane(glm::vec3& p,
                                          const glm::vec3& planePoint,
                                          const glm::vec3& planeNormal,
                                          const glm::vec3& minB,
                                          const glm::vec3& maxB)
        {
            for (int i = 0; i < 16; ++i)
            {
                p.x = std::clamp(p.x, minB.x, maxB.x);
                p.y = std::clamp(p.y, minB.y, maxB.y);
                p.z = std::clamp(p.z, minB.z, maxB.z);
                const float d = glm::dot(p - planePoint, planeNormal);
                if (std::abs(d) <= 1e-5f)
                    break;
                p -= d * planeNormal;
            }
        }
    }

    void Renderer3D::shutdown()
    {
        if (solidPlaneVbo_) { glDeleteBuffers(1, &solidPlaneVbo_); solidPlaneVbo_ = 0; }
        if (solidPlaneVao_) { glDeleteVertexArrays(1, &solidPlaneVao_); solidPlaneVao_ = 0; }
        if (solidCubeVbo_) { glDeleteBuffers(1, &solidCubeVbo_); solidCubeVbo_ = 0; }
        if (solidCubeVao_) { glDeleteVertexArrays(1, &solidCubeVao_); solidCubeVao_ = 0; }
        if (litShader_) { glDeleteProgram(litShader_); litShader_ = 0; }
        if (shadowDepthShader_) { glDeleteProgram(shadowDepthShader_); shadowDepthShader_ = 0; }
        if (shadowDepthTex_) { glDeleteTextures(1, &shadowDepthTex_); shadowDepthTex_ = 0; }
        if (shadowFbo_) { glDeleteFramebuffers(1, &shadowFbo_); shadowFbo_ = 0; }

        if (cursorSphereEbo_) { glDeleteBuffers(1, &cursorSphereEbo_); cursorSphereEbo_ = 0; }
        if (cursorSphereVbo_) { glDeleteBuffers(1, &cursorSphereVbo_); cursorSphereVbo_ = 0; }
        if (cursorSphereVao_) { glDeleteVertexArrays(1, &cursorSphereVao_); cursorSphereVao_ = 0; }
        if (cursorShader_) { glDeleteProgram(cursorShader_); cursorShader_ = 0; }
        groundGrid_.shutdown();
        cubeMesh_.shutdown();
        scene_.shutdown();
    }

    void Renderer3D::render(int width, int height)
    {
        jassert(width > 0 && height > 0);

        // Use current viewport so we don't overwrite JUCE's (component rect in framebuffer).
        // Overwriting with glViewport(0,0, width, height) can cram the scene into bottom-left.
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        const int w = (vp[2] > 0 && vp[3] > 0) ? vp[2] : width;
        const int h = (vp[2] > 0 && vp[3] > 0) ? vp[3] : height;

        lastViewportWidth_ = w;
        lastViewportHeight_ = h;

        if (vp[2] <= 0 || vp[3] <= 0)
            glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);

        const auto view = camera_.getViewMatrix();
        const auto proj = camera_.getProjectionMatrix(static_cast<float>(w),
                                                      static_cast<float>(h));

        // Models for solid cube + plane (used for shadows).
        const auto minB = cubeMesh_.getMinBounds();
        const auto maxB = cubeMesh_.getMaxBounds();
        const glm::vec3 cubeCenter = 0.5f * (minB + maxB);
        const glm::vec3 cubeHalfExtents = 0.5f * (maxB - minB);
        const glm::mat4 cubeModel =
            glm::translate(glm::mat4(1.0f), cubeCenter) *
            glm::scale(glm::mat4(1.0f), cubeHalfExtents);

        // Plane under the cube (also aligns with the grid).
        const float gridY = minB.y - 0.001f;
        const glm::vec3 centerXZ{ cubeCenter.x, 0.0f, cubeCenter.z };
        const float planeExtent = 12.0f;
        const glm::mat4 planeModel =
            glm::translate(glm::mat4(1.0f), glm::vec3(centerXZ.x, gridY, centerXZ.z)) *
            glm::scale(glm::mat4(1.0f), glm::vec3(planeExtent, 1.0f, planeExtent));

        // Directional light for shadows.
        const glm::vec3 lightDir = glm::normalize(glm::vec3(-0.55f, -1.0f, -0.35f));
        const float lightDistance = 10.0f;
        const glm::vec3 lightPos = cubeCenter - lightDir * lightDistance;
        const glm::mat4 lightView = glm::lookAt(lightPos, cubeCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        const float lightOrtho = 8.0f;
        const glm::mat4 lightProj = glm::ortho(-lightOrtho, lightOrtho, -lightOrtho, lightOrtho, 0.1f, 30.0f);
        const glm::mat4 lightSpace = lightProj * lightView;

        // 1) Shadow map pass.
        if (shadowFbo_ && shadowDepthShader_ && shadowDepthTex_ && solidCubeVao_ && solidPlaneVao_)
        {
            glViewport(0, 0, shadowMapSize, shadowMapSize);
            glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo_);
            glClear(GL_DEPTH_BUFFER_BIT);

            glUseProgram(shadowDepthShader_);
            glUniformMatrix4fv(glGetUniformLocation(shadowDepthShader_, "uLightSpace"), 1, GL_FALSE, glm::value_ptr(lightSpace));

            glBindVertexArray(solidPlaneVao_);
            glUniformMatrix4fv(glGetUniformLocation(shadowDepthShader_, "uModel"), 1, GL_FALSE, glm::value_ptr(planeModel));
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Cursor sphere also casts a shadow.
            if (cursorSphereVao_ && cursorSphereIndexCount_ > 0)
            {
                const glm::mat4 cursorModel = glm::scale(
                    glm::translate(glm::mat4(1.0f), cursorPositionCube_),
                    glm::vec3(cursorSphereRadius, cursorSphereRadius, cursorSphereRadius));
                glBindVertexArray(cursorSphereVao_);
                glUniformMatrix4fv(glGetUniformLocation(shadowDepthShader_, "uModel"), 1, GL_FALSE, glm::value_ptr(cursorModel));
                glDrawElements(GL_TRIANGLES, cursorSphereIndexCount_, GL_UNSIGNED_INT, nullptr);
            }

            glBindVertexArray(0);
            glUseProgram(0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Restore viewport for the main pass (JUCE's viewport rect).
        if (vp[2] > 0 && vp[3] > 0)
            glViewport(vp[0], vp[1], vp[2], vp[3]);
        else
            glViewport(0, 0, width, height);

        juce::OpenGLHelpers::clear(sceneClearColour);

        // Ground grid slightly below the cube bottom face to avoid z-fighting.
        groundGrid_.render(view, proj,
                           /*gridY*/ gridY,
                           /*centerXZ*/ centerXZ);

        scene_.render(view, proj);

        // 2) Lit solids with shadowing (ground plane + cube).
        if (litShader_ && shadowDepthTex_ && solidCubeVao_ && solidPlaneVao_)
        {
            glUseProgram(litShader_);
            glUniformMatrix4fv(glGetUniformLocation(litShader_, "uView"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(litShader_, "uProjection"), 1, GL_FALSE, glm::value_ptr(proj));
            glUniformMatrix4fv(glGetUniformLocation(litShader_, "uLightSpace"), 1, GL_FALSE, glm::value_ptr(lightSpace));
            glUniform3fv(glGetUniformLocation(litShader_, "uLightDir"), 1, glm::value_ptr(lightDir));
            const glm::vec3 viewPos = camera_.getPosition();
            glUniform3fv(glGetUniformLocation(litShader_, "uViewPos"), 1, glm::value_ptr(viewPos));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, shadowDepthTex_);
            glUniform1i(glGetUniformLocation(litShader_, "uShadowMap"), 0);

            // Plane
            glBindVertexArray(solidPlaneVao_);
            glUniformMatrix4fv(glGetUniformLocation(litShader_, "uModel"), 1, GL_FALSE, glm::value_ptr(planeModel));
            glUniform3f(glGetUniformLocation(litShader_, "uBaseColor"), 0.93f, 0.93f, 0.94f);
            glUniform1f(glGetUniformLocation(litShader_, "uAlpha"), 1.0f);
            glUniform1i(glGetUniformLocation(litShader_, "uUseBlobShadow"), 1);
            glUniform1f(glGetUniformLocation(litShader_, "uShadowStrength"), 0.0f);
            glUniform3fv(glGetUniformLocation(litShader_, "uCursorWorldPos"), 1, glm::value_ptr(cursorPositionCube_));
            glUniform1f(glGetUniformLocation(litShader_, "uPlaneY"), gridY);
            glUniform1f(glGetUniformLocation(litShader_, "uBlobRadiusBase"), 0.18f);
            glUniform1f(glGetUniformLocation(litShader_, "uBlobRadiusPerHeight"), 0.55f);
            glUniform1f(glGetUniformLocation(litShader_, "uBlobSoftness"), 0.20f);
            glUniform1f(glGetUniformLocation(litShader_, "uBlobStrength"), 0.48f);
            glUniform1i(glGetUniformLocation(litShader_, "uCubePulseMode"), 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Cube (develop TrilinearCube-style blue pulse + transparency)
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glBindVertexArray(solidCubeVao_);
            glUniformMatrix4fv(glGetUniformLocation(litShader_, "uModel"), 1, GL_FALSE, glm::value_ptr(cubeModel));
            glUniform3f(glGetUniformLocation(litShader_, "uBaseColor"), 0.86f, 0.36f, 0.26f);
            glUniform1f(glGetUniformLocation(litShader_, "uAlpha"), 0.35f);
            glUniform1i(glGetUniformLocation(litShader_, "uUseBlobShadow"), 0);
            glUniform1f(glGetUniformLocation(litShader_, "uShadowStrength"), 0.0f);
            glUniform1i(glGetUniformLocation(litShader_, "uCubePulseMode"), 1);
            glUniform1f(glGetUniformLocation(litShader_, "uTime"),
                        static_cast<float>(juce::Time::getMillisecondCounterHiRes() * 0.001));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUseProgram(0);
        }

        cubeMesh_.render(view, proj,
                         static_cast<float>(juce::Time::getMillisecondCounterHiRes() * 0.001));

        // Corner spheres (depth tested, so they sit in 3D correctly)
        if (cursorShader_ && cursorSphereVao_ && cursorSphereIndexCount_ > 0)
        {
            const glm::vec3 corners[8] = {
                { minB.x, minB.y, minB.z }, // 0
                { maxB.x, minB.y, minB.z }, // 1
                { minB.x, maxB.y, minB.z }, // 2
                { maxB.x, maxB.y, minB.z }, // 3
                { minB.x, minB.y, maxB.z }, // 4
                { maxB.x, minB.y, maxB.z }, // 5
                { minB.x, maxB.y, maxB.z }, // 6
                { maxB.x, maxB.y, maxB.z }, // 7
            };

            glUseProgram(cursorShader_);
            glUniformMatrix4fv(glGetUniformLocation(cursorShader_, "uView"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(cursorShader_, "uProjection"), 1, GL_FALSE, glm::value_ptr(proj));
            glBindVertexArray(cursorSphereVao_);

            for (int i = 0; i < 8; ++i)
            {
                const glm::mat4 model = glm::scale(
                    glm::translate(glm::mat4(1.0f), corners[i]),
                    glm::vec3(cornerSphereRadius, cornerSphereRadius, cornerSphereRadius));
                glUniformMatrix4fv(glGetUniformLocation(cursorShader_, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
                glUniform4fv(glGetUniformLocation(cursorShader_, "uColor"), 1, glm::value_ptr(cornerColours_[static_cast<size_t>(i)]));
                glDrawElements(GL_TRIANGLES, cursorSphereIndexCount_, GL_UNSIGNED_INT, nullptr);
            }

            glBindVertexArray(0);
            glUseProgram(0);
        }

        // Cursor sphere in same world space as cube (depth tested so it can be occluded).
        if (cursorShader_ && cursorSphereVao_ && cursorSphereIndexCount_ > 0)
        {
            const glm::mat4 model = glm::scale(
                glm::translate(glm::mat4(1.0f), cursorPositionCube_),
                glm::vec3(cursorSphereRadius, cursorSphereRadius, cursorSphereRadius));
            glUseProgram(cursorShader_);
            glUniformMatrix4fv(glGetUniformLocation(cursorShader_, "uView"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(glGetUniformLocation(cursorShader_, "uProjection"), 1, GL_FALSE, glm::value_ptr(proj));
            glUniformMatrix4fv(glGetUniformLocation(cursorShader_, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
            glUniform4fv(glGetUniformLocation(cursorShader_, "uColor"), 1, glm::value_ptr(cursorColour));
            glBindVertexArray(cursorSphereVao_);
            glDrawElements(GL_TRIANGLES, cursorSphereIndexCount_, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
            glUseProgram(0);
        }
    }

    void Renderer3D::mouseDown(const juce::MouseEvent& e, const juce::Rectangle<int>& viewportBounds)
    {
        lastMousePosition_ = e.getPosition();
        isDragging_ = true;

        const int vpW = viewportBounds.getWidth();
        const int vpH = viewportBounds.getHeight();
        if (vpW <= 0 || vpH <= 0 || e.mods.isShiftDown())
            return;

        const int relX = e.getPosition().x - viewportBounds.getX();
        const int relY = e.getPosition().y - viewportBounds.getY();
        const float screenX = static_cast<float>(relX) / static_cast<float>(vpW);
        const float screenY = static_cast<float>(relY) / static_cast<float>(vpH);

        const auto ray = rayCaster_.castRay(screenX, screenY,
                                            static_cast<float>(vpW),
                                            static_cast<float>(vpH));
        glm::vec3 hitPoint{};
        if (rayCaster_.intersectCube(ray,
                                    cubeMesh_.getMinBounds(),
                                    cubeMesh_.getMaxBounds(),
                                    hitPoint))
        {
            cursorPositionCube_ = hitPoint;
            clampCursorToCube();
        }
    }

    void Renderer3D::mouseDrag(const juce::MouseEvent& e, const juce::Rectangle<int>& viewportBounds)
    {
        if (!isDragging_)
            return;

        const auto pos = e.getPosition();
        const auto delta = pos - lastMousePosition_;
        lastMousePosition_ = pos;

        const int vpW = viewportBounds.getWidth();
        const int vpH = viewportBounds.getHeight();
        if (vpW <= 0 || vpH <= 0)
            return;

        if (e.mods.isShiftDown())
        {
            camera_.orbit(static_cast<float>(delta.x),
                          static_cast<float>(delta.y));
        }
        else
        {
            // Use position relative to viewport so ray cast matches GL viewport.
            const int relX = pos.x - viewportBounds.getX();
            const int relY = pos.y - viewportBounds.getY();
            const float screenX = static_cast<float>(relX) / static_cast<float>(vpW);
            const float screenY = static_cast<float>(relY) / static_cast<float>(vpH);

            const auto ray = rayCaster_.castRay(screenX, screenY,
                                                static_cast<float>(vpW),
                                                static_cast<float>(vpH));

            // Slide on the plane through the current cursor, perpendicular to view (no depth drift).
            const glm::vec3 planeNormal = camera_.getForward();
            const glm::vec3 planePoint = cursorPositionCube_;
            const auto minB = cubeMesh_.getMinBounds();
            const auto maxB = cubeMesh_.getMaxBounds();

            glm::vec3 hitPoint{};
            if (intersectRayPlane(ray, planePoint, planeNormal, hitPoint))
            {
                cursorPositionCube_ = hitPoint;
                clampCursorToCubeOnViewPlane(cursorPositionCube_, planePoint, planeNormal, minB, maxB);
            }
            else if (rayCaster_.intersectCube(ray, minB, maxB, hitPoint))
            {
                cursorPositionCube_ = hitPoint;
                projectOntoPlane(cursorPositionCube_, planePoint, planeNormal);
                clampCursorToCubeOnViewPlane(cursorPositionCube_, planePoint, planeNormal, minB, maxB);
            }
        }
    }

    void Renderer3D::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
    {
        juce::ignoreUnused(e);
        camera_.zoom(wheel.deltaY);
    }

    void Renderer3D::clampCursorToCube()
    {
        const auto minB = cubeMesh_.getMinBounds();
        const auto maxB = cubeMesh_.getMaxBounds();
        cursorPositionCube_.x = std::clamp(cursorPositionCube_.x, minB.x, maxB.x);
        cursorPositionCube_.y = std::clamp(cursorPositionCube_.y, minB.y, maxB.y);
        cursorPositionCube_.z = std::clamp(cursorPositionCube_.z, minB.z, maxB.z);
    }

    glm::vec3 Renderer3D::unitToCube(const glm::vec3& unitPos) const
    {
        const auto minB = cubeMesh_.getMinBounds();
        const auto maxB = cubeMesh_.getMaxBounds();
        return {
            minB.x + unitPos.x * (maxB.x - minB.x),
            minB.y + unitPos.y * (maxB.y - minB.y),
            minB.z + unitPos.z * (maxB.z - minB.z)
        };
    }

    glm::vec3 Renderer3D::cubeToUnit(const glm::vec3& cubePos) const
    {
        const auto minB = cubeMesh_.getMinBounds();
        const auto maxB = cubeMesh_.getMaxBounds();

        const auto extent = maxB - minB;

        glm::vec3 result{
            extent.x != 0.0f ? (cubePos.x - minB.x) / extent.x : 0.0f,
            extent.y != 0.0f ? (cubePos.y - minB.y) / extent.y : 0.0f,
            extent.z != 0.0f ? (cubePos.z - minB.z) / extent.z : 0.0f
        };

        result.x = std::clamp(result.x, 0.0f, 1.0f);
        result.y = std::clamp(result.y, 0.0f, 1.0f);
        result.z = std::clamp(result.z, 0.0f, 1.0f);

        return result;
    }

    void Renderer3D::setCursorFromUnitPosition(const glm::vec3& unitPos)
    {
        cursorPositionCube_ = unitToCube(unitPos);
        clampCursorToCube();
    }

    glm::vec3 Renderer3D::getCursorAsUnitPosition() const
    {
        return cubeToUnit(cursorPositionCube_);
    }

    void Renderer3D::setCornerColours(const std::array<glm::vec4, 8>& colours)
    {
        cornerColours_ = colours;
    }
}

