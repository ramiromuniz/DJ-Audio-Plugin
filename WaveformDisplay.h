#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"


//==============================================================================
/*
*/
class WaveformDisplay : public juce::Component,
    public juce::ChangeListener,
    public juce::MouseListener
{
public:
    WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
        juce::AudioThumbnailCache& cacheToUse,
        DJAudioPlayer* playerToUse);
    ~WaveformDisplay() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void loadURL(juce::URL audioURL);

    /** set the relatuve position of playhead */
    void setPositionRelative(double pos);

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void updatePlayheadPositionFromMouseEvent(const juce::MouseEvent& event);


private:

    juce::Colour playedWaveformColor;
    juce::Colour unplayedWaveformColor;
    juce::Image thumbnailImage;

    DJAudioPlayer* player; // Add the DJAudioPlayer pointer as a member variable

    juce::AudioThumbnail audioThumb;

    bool fileLoaded;
    double position;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformDisplay)
};
