#pragma once

#include <juce_opengl/juce_opengl.h>
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

    private:
        Scene scene_;
        Camera camera_;

        CubeMesh cubeMesh_;
        GroundGrid groundGrid_;
        RayCaster rayCaster_{ &camera_ };
        TransformGizmo gizmo_;

        glm::vec3 cursorPositionCube_{ 0.0f, 0.0f, 0.0f };

        std::array<glm::vec4, 8> cornerColours_{};

        int lastViewportWidth_{ 1 };
        int lastViewportHeight_{ 1 };

        juce::Point<int> lastMousePosition_;
        bool isDragging_{ false };

        // Gizmo drag state.
        TransformGizmo::Part activePart_{ TransformGizmo::Part::None };
        glm::vec3            dragCursorStart_{};

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

        void clampCursorToCube();
        glm::vec3 unitToCube(const glm::vec3& unitPos) const;
        glm::vec3 cubeToUnit(const glm::vec3& cubePos) const;
        void applyGizmoDrag(const Ray& ray);

    public:
        void setCursorFromUnitPosition(const glm::vec3& unitPos);
        glm::vec3 getCursorAsUnitPosition() const;

        void setCornerColours(const std::array<glm::vec4, 8>& colours);

        /** True while any mouse button is held (gizmo or free drag). */
        bool hasActiveDrag() const noexcept { return isDragging_; }
    };
}

