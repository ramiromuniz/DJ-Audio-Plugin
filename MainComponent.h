#pragma once

#include <JuceHeader.h>
#include "DJAudioPlayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;

    /** Implement Button::Listener */
    void buttonClicked(juce::Button*) override;

    /** Implement Slider::Listener */
    void sliderValueChanged(juce::Slider* slider) override;
    //initialise the load playlist function 
    void initialise();

    

private:
    //==============================================================================
    // Your private member variables go here...
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbCache{ 100 };

    DJAudioPlayer player1{ formatManager };
    DJAudioPlayer player2{ formatManager };


    DeckGUI deckGUI1{ &player1, formatManager, thumbCache };
    DeckGUI deckGUI2{ &player2, formatManager, thumbCache };


    //crossfader Slider
    juce::Slider crossFaderSlider;
    juce::Label crossFaderLabel;



    juce::MixerAudioSource mixerSource;

    PlaylistComponent playlistComponents{ &deckGUI1, &deckGUI2, formatManager, thumbCache };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
