//
// Created by Angelina Zhai on 2026-02-20.
//

#include "MidiManager.h"

std::vector<std::string> MidiManager::getInputDeviceNames() const
{
    std::vector<std::string> names;
    for (auto& d : juce::MidiInput::getAvailableDevices())
        names.push_back(d.name.toStdString());
    return names;
}

bool MidiManager::openInput(int deviceIndex) {

    // in case auto doesn't work, type = juce::Array<juce::MidiDeviceInfo>
    auto devices = juce::MidiInput::getAvailableDevices();

    // cornercase checking + type casting unsigned int
    if (deviceIndex < 0 || deviceIndex >= static_cast<int>(devices.size()))
        return false;

    // close opened midi device (if exists)
    closeMIDI();

    // save opened device to private ptr
    midiInput = juce::MidiInput::openDevice(devices[deviceIndex].identifier, this);

    if (midiInput == nullptr)
        return false;

    // start device
    midiInput->start();
    return true;

}

void MidiManager::closeMIDI()
{
    if (midiInput != nullptr) {
        midiInput -> stop();
        midiInput = nullptr;
    }
}