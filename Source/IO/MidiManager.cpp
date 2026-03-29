//
// Created by Angelina Zhai on 2026-02-20.
//

#include "MidiManager.h"
#include "Audio/SynthEngine.h"

namespace IO
{

MidiManager::MidiManager (Audio::SynthEngine& synthEngine) noexcept
    : synthEngine_ (synthEngine)
{
}

void MidiManager::setMidiChannel (int channel) noexcept
{
    if (channel < 0 || channel > 16)
        midiChannel_ = 0; // fallback to omni
    else
        midiChannel_ = channel;
}

bool MidiManager::channelMatches (int messageChannel) const noexcept
{
    if (midiChannel_ == 0)
        return true;

    return messageChannel == midiChannel_;
}

void MidiManager::processMidiBuffer (const juce::MidiBuffer& midi)
{
    for (const auto metadata : midi)
    {
        const auto& msg = metadata.getMessage();
        const int channel = msg.getChannel();

        if (! channelMatches (channel))
            continue;

        if (msg.isNoteOn())
        {
            handleNoteOn (channel, msg.getNoteNumber(), msg.getFloatVelocity());
        }
        else if (msg.isNoteOff())
        {
            handleNoteOff (channel, msg.getNoteNumber(), msg.getFloatVelocity());
        }
        else if (msg.isPitchWheel())
        {
            handlePitchWheel (channel, msg.getPitchWheelValue());
        }
        else if (msg.isController())
        {
            handleController (channel, msg.getControllerNumber(), msg.getControllerValue());
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            synthEngine_.allNotesOff();
        }
    }
}

void MidiManager::handleNoteOn (int channel, int noteNumber, float velocity)
{
    synthEngine_.noteOn (channel, noteNumber, velocity);
}

void MidiManager::handleNoteOff (int channel, int noteNumber, float velocity)
{
    synthEngine_.noteOff (channel, noteNumber, velocity, true);
}

void MidiManager::handlePitchWheel (int /*channel*/, int value)
{
    synthEngine_.pitchWheelMoved (value);
}

void MidiManager::handleController (int /*channel*/, int controller, int value)
{
    synthEngine_.controllerMoved (controller, value);
}

} // namespace IO

