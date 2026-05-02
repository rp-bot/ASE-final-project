#pragma once

namespace DSP
{
    /** Cheaper than std::tanh; Padé-style rational, clamped to ±1 for large |x|. */
    inline float fastTanh(float x) noexcept
    {
        const float x2 = x * x;
        float r = x * (27.0f + x2) / (27.0f + 9.0f * x2);
        if (r > 1.0f)
            r = 1.0f;
        else if (r < -1.0f)
            r = -1.0f;
        return r;
    }
}
