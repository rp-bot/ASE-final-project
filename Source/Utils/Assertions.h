#pragma once

// Use JUCE's jassert in debug; nothing in release.
#include <juce_core/juce_core.h>

//==============================================================================
// Volumetric assertion macros (global scope; VOLUMETRIC_* prefix).
// Use for debug-only checks. Do not rely on assertions for correctness in Release.
// In Release, all macros compile out and have no side effects (audio-thread safe when disabled).
// Prefer jassert() for consistency; use these when you need a custom message or a named RT assert.
//==============================================================================

#if JUCE_DEBUG
    /** Use in place of raw jassert() when you want a consistent project prefix. */
    #define VOLUMETRIC_ASSERT(condition)           jassert(condition)
    /** Like VOLUMETRIC_ASSERT but logs msg (via DBG) before triggering. Do not use on audio thread if DBG may allocate. */
    #define VOLUMETRIC_ASSERT_MSG(condition, msg)  do { if (!(condition)) { DBG(msg); jassertfalse; } } while (0)
    /** Real-time safe when compiled out. Use only for audio-thread checks compiled out in Release; no side effects. */
    #define VOLUMETRIC_RT_ASSERT(condition)       jassert(condition)
#else
    #define VOLUMETRIC_ASSERT(condition)           ((void)0)
    #define VOLUMETRIC_ASSERT_MSG(condition, msg)  ((void)0)
    #define VOLUMETRIC_RT_ASSERT(condition)       ((void)0)
#endif
