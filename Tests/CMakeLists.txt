
juce_add_console_app(VolumetricSynth_Tests
    PRODUCT_NAME "Volumetric Synth Tests"
)

target_sources(VolumetricSynth_Tests
    PRIVATE
    Main.cpp
    UnitTests/SampleTest.cpp
)

target_link_libraries(VolumetricSynth_Tests
    PRIVATE
    VolumetricSynth
    juce::juce_audio_plugin_client
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Ensure C++ standard matches
target_compile_features(VolumetricSynth_Tests PRIVATE cxx_std_17)

# Generate JuceHeader.h for the test target
juce_generate_juce_header(VolumetricSynth_Tests)
