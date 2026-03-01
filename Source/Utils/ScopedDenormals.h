#pragma once

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    #include <xmmintrin.h>
#endif

namespace Utils
{
    /**
     * RAII guard that disables denormal handling for the current thread
     * for the lifetime of the object. On destruction, restores the previous
     * FP mode. Use at the start of processBlock() (or similar) on the audio thread
     * to avoid denormal-induced CPU spikes and audio glitches.
     *
     * Platform behavior:
     * - x86/x64: Set DAZ+FTZ (denormals are zero) via MXCSR.
     * - ARM: No-op (no crash).
     * - Other: No-op (no crash).
     */
    class ScopedDenormals
    {
    public:
        ScopedDenormals() noexcept;
        ~ScopedDenormals() noexcept;

        ScopedDenormals(const ScopedDenormals&) = delete;
        ScopedDenormals& operator=(const ScopedDenormals&) = delete;
        ScopedDenormals(ScopedDenormals&&) = delete;
        ScopedDenormals& operator=(ScopedDenormals&&) = delete;

    private:
        unsigned int m_previousState { 0 };
        bool m_restore { false };
    };
}

//==============================================================================
// Implementation (header-only)
//==============================================================================

namespace Utils
{
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    // MXCSR bits: FTZ = 0x8000, DAZ = 0x40
    static constexpr unsigned int kDenormalsOffMask = 0x8040u;

    inline ScopedDenormals::ScopedDenormals() noexcept
    {
        m_previousState = static_cast<unsigned int>(_mm_getcsr());
        unsigned int withFtzDaz = m_previousState | kDenormalsOffMask;
        if (withFtzDaz != m_previousState)
        {
            _mm_setcsr(withFtzDaz);
            m_restore = true;
        }
    }

    inline ScopedDenormals::~ScopedDenormals() noexcept
    {
        if (m_restore)
            _mm_setcsr(m_previousState);
    }
#else
    inline ScopedDenormals::ScopedDenormals() noexcept = default;

    inline ScopedDenormals::~ScopedDenormals() noexcept = default;
#endif
}
