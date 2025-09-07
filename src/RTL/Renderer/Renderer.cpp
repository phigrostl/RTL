#include "Renderer.h"

namespace RTL {

    bool Renderer::IsVertexVisible(const Vec4& clipPos)
    {
        return fabs(clipPos.X) <= clipPos.W && fabs(clipPos.Y) <= clipPos.W && fabs(clipPos.Z) <= clipPos.W;
    }

    bool Renderer::IsInsidePlane(const Vec4& clipPos, const Plane plane) {
        switch (plane)
        {
        case Plane::POSITIVE_X:
            return clipPos.X <= +clipPos.W;
        case Plane::NEGATIVE_X:
            return clipPos.X >= -clipPos.W;
        case Plane::POSITIVE_Y:
            return clipPos.Y <= +clipPos.W;
        case Plane::NEGATIVE_Y:
            return clipPos.Y >= -clipPos.W;
        case Plane::POSITIVE_Z:
            return clipPos.Z <= +clipPos.W;
        case Plane::NEGATIVE_Z:
            return clipPos.Z >= -clipPos.W;
        default:
            return false;
        }
    }

    float Renderer::GetIntersectRatio(const Vec4& prev, const Vec4& curr, const Plane plane) {
        switch (plane) {
        case Plane::POSITIVE_X:
            return (prev.W - prev.X) / ((prev.W - prev.X) - (curr.W - curr.X));
        case Plane::NEGATIVE_X:
            return (prev.W + prev.X) / ((prev.W + prev.X) - (curr.W + curr.X));
        case Plane::POSITIVE_Y:
            return (prev.W - prev.Y) / ((prev.W - prev.Y) - (curr.W - curr.Y));
        case Plane::NEGATIVE_Y:
            return (prev.W + prev.Y) / ((prev.W + prev.Y) - (curr.W + curr.Y));
        case Plane::POSITIVE_Z:
            return (prev.W - prev.Z) / ((prev.W - prev.Z) - (curr.W - curr.Z));
        case Plane::NEGATIVE_Z:
            return (prev.W + prev.Z) / ((prev.W + prev.Z) - (curr.W + curr.Z));
        default:
            return 0.0f;
        }
    }

}
