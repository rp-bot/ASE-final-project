
juce_add_console_app(VolumetricSynth_Tests
    PRODUCT_NAME "Volumetric Synth Tests"
)

target_sources(VolumetricSynth_Tests
    PRIVATE
    Main.cpp
    UnitTests/AssertionsTest.cpp
    UnitTests/AtomicGuiStateTest.cpp
    UnitTests/SampleTest.cpp
    UnitTests/Math3DTest.cpp
    UnitTests/ScopedDenormalsTest.cpp
    UnitTests/DualFilterTest.cpp
    UnitTests/TrilinearMixer8Test.cpp
)

target_include_directories(VolumetricSynth_Tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/Source
)

target_link_libraries(VolumetricSynth_Tests
    PRIVATE
    VolumetricSynth
    glm::glm
    juce::juce_audio_plugin_client
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Ensure C++ standard matches
target_compile_features(VolumetricSynth_Tests PRIVATE cxx_std_17)

# Generate JuceHeader.h for the test target
juce_generate_juce_header(VolumetricSynth_Tests)
