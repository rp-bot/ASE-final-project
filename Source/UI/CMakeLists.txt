target_sources("${PROJECT_NAME}"
    PRIVATE
        LabeledPanel.h
        LabeledPanel.cpp
        OscillatorModuleComponent.h
        OscillatorModuleComponent.cpp
        CenterControlPanel.h
        CenterControlPanel.cpp
        # TRILINEAR MIXER VISUALIZATION (remove these two lines when removing visualization)
        TrilinearMixerGainsView.h
        TrilinearMixerGainsView.cpp)
