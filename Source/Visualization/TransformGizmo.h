#pragma once

#include <glm/glm.hpp>
#include "RayCaster.h"

namespace Visualization
{
    /**
     * Unity-style transform gizmo rendered at the cursor position.
     *
     * Three axis arrows (X=red, Y=green, Z=blue) allow single-axis movement.
     * Three plane squares (XY=blue, XZ=green, YZ=red) allow two-axis movement.
     *
     * All geometry is built in gizmo-local space on [0,1] and scaled by
     * `gizmoScale` (typically camera.radius * kGizmoScale) so the gizmo
     * maintains a constant apparent size in screen space regardless of zoom.
     */
    class TransformGizmo
    {
    public:
        enum class Part { None, AxisX, AxisY, AxisZ, PlaneXY, PlaneXZ, PlaneYZ };

        void initialise();
        void shutdown();

        /**
         * Draw the gizmo centred at `origin`.
         * `activePart` receives a yellow highlight so the user can see which
         * axis/plane is currently being dragged.
         */
        void render(const glm::vec3& origin,
                    float            gizmoScale,
                    const glm::mat4& view,
                    const glm::mat4& proj,
                    Part             activePart = Part::None);

        /** Returns the first gizmo part hit by `ray`, or Part::None. */
        Part hitTest(const Ray&       ray,
                     const glm::vec3& origin,
                     float            gizmoScale) const noexcept;

    private:
        unsigned int shader_{ 0 };

        // Arrow (cylinder shaft + cone tip) mesh along +Y, local y ∈ [0, 1].
        unsigned int arrowVao_{ 0 };
        unsigned int arrowVbo_{ 0 };
        unsigned int arrowEbo_{ 0 };
        int          arrowIndexCount_{ 0 };

        // Unit quad in the XY plane, local coords ∈ [kQuadOffset, kQuadOffset+kQuadSize].
        unsigned int quadVao_{ 0 };
        unsigned int quadVbo_{ 0 };
        unsigned int quadEbo_{ 0 };

        void buildArrowGeo();
        void buildQuadGeo();

        void drawMesh(unsigned int     vao,
                      int              indexCount,
                      const glm::mat4& model,
                      const glm::vec4& color,
                      const glm::mat4& view,
                      const glm::mat4& proj) const noexcept;

        static bool hitCapsule(const Ray&       ray,
                               const glm::vec3& p0,
                               const glm::vec3& p1,
                               float            radius) noexcept;

        static bool hitRect(const Ray&       ray,
                            const glm::vec3& v0,
                            const glm::vec3& v1,
                            const glm::vec3& v2,
                            const glm::vec3& v3) noexcept;
    };
}
