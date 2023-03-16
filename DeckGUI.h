#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"


//==============================================================================
/*
*/
class DeckGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::FileDragAndDropTarget,
    public juce::Timer,
    public juce::ChangeListener
{
public:

    DeckGUI(DJAudioPlayer* player,
        juce::AudioFormatManager& formatManagerToUse,
        juce::AudioThumbnailCache& cacheToUse);
    ~DeckGUI() override;


    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void paint(juce::Graphics&) override;
    void resized() override;

    /** Implement Button::Listener */
    void buttonClicked(juce::Button*) override;

    /** Implement Slider::Listener */
    void sliderValueChanged(juce::Slider* slider) override;


    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;


    void timerCallback() override;

    juce::String formatTime(double seconds);


    DJAudioPlayer* player;
    WaveformDisplay waveformDisplay;
private:

   
    juce::Label currentTimeLabel;

    juce::TextButton playButton{ "PLAY" };
    juce::TextButton stopButton{ "STOP" };


    juce::Slider volSlider;
    juce::Slider speedSlider;

    juce::Label volSliderLabel;
    juce::Label speedSliderLabel;


    juce::FileChooser fChooser{ "Select a file..." };


    juce::File loadedFile;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
};
