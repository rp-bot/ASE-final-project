#pragma once

// JUCE modules
#include <juce_core/juce_core.h>           // String, uint8_t, etc.
#include <juce_audio_basics/juce_audio_basics.h>   // MidiMessage
#include <juce_audio_devices/juce_audio_devices.h> // MidiInput, MidiInputCallback

// Standard library
#include <vector>
#include <string>
#include <memory>

struct MidiEvent
{
    enum class Type { NoteOn, NoteOff, Other };

    Type type;
    uint8_t channel; //instrument channel (0-15), having this just in case
    uint8_t note; //0-127
    uint8_t velocity; //0-127
    uint32_t timestamp;

};



class MidiManager : private juce::MidiInputCallback
{
public:

    MidiManager() = default;
    ~MidiManager() { closeMIDI(); }

    // get input device names
    // returns: vector of all input device names
    std::vector<std::string> getInputDeviceNames() const; //const: ensures the midi devices not modified

    // opens device at specified deviceIndex based on avaliable devices list
    // returns: bool (status for success)
    bool openInput(int deviceIndex);

    // closes saved midi device & set to null
    void closeMIDI();

private:
    std::unique_ptr<juce::MidiInput> midiInput = nullptr;
};