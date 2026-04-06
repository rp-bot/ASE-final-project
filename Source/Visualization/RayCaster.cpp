#include "RayCaster.h"

#include "Camera.h"
#include <juce_core/juce_core.h>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Visualization
{
    RayCaster::RayCaster(const Camera* camera) noexcept
        : camera_(camera)
    {
    }

    void RayCaster::setCamera(const Camera* camera) noexcept
    {
        camera_ = camera;
    }

    Ray RayCaster::castRay(float screenX,
                           float screenY,
                           float viewportWidth,
                           float viewportHeight) const
    {
        jassert(camera_ != nullptr);
        jassert(viewportWidth > 0.0f && viewportHeight > 0.0f);

        const auto view = camera_->getViewMatrix();
        const auto proj = camera_->getProjectionMatrix(viewportWidth, viewportHeight);

        const auto invView = glm::inverse(view);
        const auto invProj = glm::inverse(proj);

        // Convert from [0,1] to Normalised Device Coordinates [-1,1]
        const float ndcX = 2.0f * screenX - 1.0f;
        const float ndcY = 1.0f - 2.0f * screenY; // invert Y to match GL

        const glm::vec4 rayStartClip{ ndcX, ndcY, -1.0f, 1.0f };
        const glm::vec4 rayEndClip  { ndcX, ndcY,  1.0f, 1.0f };

        const auto rayStartView = invProj * rayStartClip;
        const auto rayEndView   = invProj * rayEndClip;

        const glm::vec3 startView(rayStartView.x / rayStartView.w,
                                  rayStartView.y / rayStartView.w,
                                  rayStartView.z / rayStartView.w);
        const glm::vec3 endView(rayEndView.x / rayEndView.w,
                                rayEndView.y / rayEndView.w,
                                rayEndView.z / rayEndView.w);

        const glm::vec4 rayStartWorld = invView * glm::vec4(startView, 1.0f);
        const glm::vec4 rayEndWorld   = invView * glm::vec4(endView, 1.0f);

        const glm::vec3 origin{ rayStartWorld };
        const glm::vec3 end{ rayEndWorld };
        const glm::vec3 direction = glm::normalize(end - origin);

        return { origin, direction };
    }

    bool RayCaster::intersectCube(const Ray& ray,
                                  const glm::vec3& minBounds,
                                  const glm::vec3& maxBounds,
                                  glm::vec3& hitPoint) const noexcept
    {
        const glm::vec3 invDir{
            ray.direction.x != 0.0f ? 1.0f / ray.direction.x : std::numeric_limits<float>::infinity(),
            ray.direction.y != 0.0f ? 1.0f / ray.direction.y : std::numeric_limits<float>::infinity(),
            ray.direction.z != 0.0f ? 1.0f / ray.direction.z : std::numeric_limits<float>::infinity()
        };

        float t1 = (minBounds.x - ray.origin.x) * invDir.x;
        float t2 = (maxBounds.x - ray.origin.x) * invDir.x;
        float t3 = (minBounds.y - ray.origin.y) * invDir.y;
        float t4 = (maxBounds.y - ray.origin.y) * invDir.y;
        float t5 = (minBounds.z - ray.origin.z) * invDir.z;
        float t6 = (maxBounds.z - ray.origin.z) * invDir.z;

        const float tMin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)),
                                    std::min(t5, t6));
        const float tMax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)),
                                    std::max(t5, t6));

        const bool intersects = (tMax >= std::max(tMin, 0.0f));

        if (intersects)
        {
            // Use midpoint of the ray segment inside the cube so the cursor stays
            // inside and moves smoothly instead of sticking to the front face/edge.
            const float tEntry = std::max(0.0f, tMin);
            const float tExit = tMax;
            const float tMid = 0.5f * (tEntry + tExit);
            glm::vec3 point = ray.origin + tMid * ray.direction;
            point.x = std::clamp(point.x, minBounds.x, maxBounds.x);
            point.y = std::clamp(point.y, minBounds.y, maxBounds.y);
            point.z = std::clamp(point.z, minBounds.z, maxBounds.z);
            hitPoint = point;
        }
        else
        {
            // Ray missed: project to nearest point on cube so the cursor still moves
            // (may sit on face/edge when dragging outside the cube).
            const float tHit = std::max(0.0f, tMin);
            glm::vec3 point = ray.origin + tHit * ray.direction;
            point.x = std::clamp(point.x, minBounds.x, maxBounds.x);
            point.y = std::clamp(point.y, minBounds.y, maxBounds.y);
            point.z = std::clamp(point.z, minBounds.z, maxBounds.z);
            hitPoint = point;
        }
        return intersects;
    }
}

