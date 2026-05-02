#pragma once

#include <juce_opengl/juce_opengl.h>
#include <atomic>
#include <glm/gtc/quaternion.hpp>
#include "Scene.h"
#include "Camera.h"
#include "CubeMesh.h"
#include "RayCaster.h"
#include "GroundGrid.h"
#include "TransformGizmo.h"
#include <array>

namespace Visualization
{
    class Renderer3D
    {
    public:
        Renderer3D() = default;
        ~Renderer3D() = default;

        void initialise();
        void shutdown();

        void render(int width, int height);

        /** viewportBounds: GL viewport rect in editor coordinates (for converting event position). */
        void mouseDown(const juce::MouseEvent& e, const juce::Rectangle<int>& viewportBounds);
        void mouseDrag(const juce::MouseEvent& e, const juce::Rectangle<int>& viewportBounds);
        void mouseUp(const juce::MouseEvent& e);
        void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel);
        void cancelInteraction() noexcept;

        /** Cube-local [0,1]^3 (follows rotated cube axes). See global variants for APVTS. */
        void setCursorFromUnitPosition(const glm::vec3& unitPos);
        glm::vec3 getCursorAsUnitPosition() const;

        /** World/global [0,1]^3 vs cube centre (axis-aligned; not co-rotated with the cube mesh). */
        void setCursorFromGlobalUnitPosition(const glm::vec3& globalUnitPos);
        glm::vec3 getCursorAsGlobalUnitPosition() const;

        glm::quat getCubeRotationQuat() const noexcept;

        void setCornerColours(const std::array<glm::vec4, 8>& colours);
        void setCameraZoom(float radius);
        float getCameraZoom() const;
        void setGizmoVisible(bool visible) noexcept { gizmoVisible_.store(visible, std::memory_order_relaxed); }
        bool isGizmoVisible() const noexcept { return gizmoVisible_.load(std::memory_order_relaxed); }
        void setAngularVelocity(const glm::vec3& velocity) noexcept { angularVelocity_ = velocity; }
        glm::vec3 getAngularVelocity() const noexcept { return angularVelocity_; }

        void setZeroGravity(bool enabled) noexcept { zeroGravity_.store(enabled, std::memory_order_relaxed); }
        bool isSpinActive() const noexcept;
        bool isAltSpinDragging() const noexcept { return isAltSpinDragging_; }

        /** True while any mouse button is held (gizmo or free drag). */
        bool hasActiveDrag() const noexcept { return isDragging_; }

    private:
        Scene scene_;
        Camera camera_;

        CubeMesh cubeMesh_;
        GroundGrid groundGrid_;
        RayCaster rayCaster_{ &camera_ };
        TransformGizmo gizmo_;

        /** World-space cursor position (does not rotate with the cube). */
        glm::vec3 cursorPositionCube_{ 0.0f, 1.5f, 0.0f };

        std::array<glm::vec4, 8> cornerColours_{};

        int lastViewportWidth_{ 1 };
        int lastViewportHeight_{ 1 };

        juce::Point<int> lastMousePosition_;
        bool isDragging_{ false };

        // Gizmo drag state.
        TransformGizmo::Part activePart_{ TransformGizmo::Part::None };
        glm::vec3            dragCursorStart_{};

        glm::quat cubeRotation_{ 1.0f, 0.0f, 0.0f, 0.0f };
        glm::vec3 angularVelocity_{ 0.0f, 0.0f, 0.0f };
        std::atomic<bool> zeroGravity_{ false };
        std::atomic<bool> gizmoVisible_{ true };

        bool isAltSpinDragging_{ false };
        glm::vec2 lastAltDeltaPixels_{ 0.0f, 0.0f };
        double    lastAltDragTimeMs_{ 0.0 };
        double    lastAltDragIntervalMs_{ 16.0 };

        double lastRenderTimeMs_{ 0.0 };

        static constexpr float kCubeElevation = 2.0f;
        static constexpr float kGroundY = 0.0f;
        glm::vec3 cubeWorldCenter_{ 0.0f, kCubeElevation, 0.0f };

        unsigned int cursorSphereVao_{ 0 };
        unsigned int cursorSphereVbo_{ 0 };
        unsigned int cursorSphereEbo_{ 0 };
        int cursorSphereIndexCount_{ 0 };
        unsigned int cursorShader_{ 0 };

        // Shadow mapping + simple lit solids (cube + ground plane).
        unsigned int shadowFbo_{ 0 };
        unsigned int shadowDepthTex_{ 0 };
        unsigned int shadowDepthShader_{ 0 };
        unsigned int litShader_{ 0 };
        unsigned int solidCubeVao_{ 0 };
        unsigned int solidCubeVbo_{ 0 };
        unsigned int solidPlaneVao_{ 0 };
        unsigned int solidPlaneVbo_{ 0 };

        glm::mat3 getCubeRotationMat() const;
        Ray       worldRayToCubeLocal(const Ray& worldRay) const;

        void clampCursorToCube();
        void clampWorldCursorToCubeOnViewPlane(glm::vec3& p,
                                               const glm::vec3& planePoint,
                                               const glm::vec3& planeNormal);
        glm::vec3 unitToLocalFromBounds(const glm::vec3& unitPos) const;
        glm::vec3 localToUnitFromBounds(const glm::vec3& localPos) const;
        void applyGizmoDrag(const Ray& ray);

        void integrateCubePhysics(float dtSec);
        glm::vec3 cameraRightForSpin() const;
        void applyAltSpinFromPixelDelta(const glm::vec2& deltaPixels);
    };
}

