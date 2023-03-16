#include <JuceHeader.h>
#include "DeckGUI.h"
#include <juce_core/juce_core.h>
#include <thread>



//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer* _player,
    juce::AudioFormatManager& formatManagerToUse,
    juce::AudioThumbnailCache& cacheToUse)
    : player(_player),
    waveformDisplay(formatManagerToUse, cacheToUse, player)
{
    addAndMakeVisible(currentTimeLabel);
    player->transportSource.addChangeListener(this);


    // Add the play, stop, and load buttons to the DeckGUI component and make them visible
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    // Add the volume, speed, and position sliders to the DeckGUI component and make them visible
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    // Add the waveformDisplay to the DeckGUI component and make it visible
    addAndMakeVisible(waveformDisplay);

    // Add the DeckGUI component as a listener for the play, stop, and load buttons
    playButton.addListener(this);
    stopButton.addListener(this);

    volSlider.addListener(this);
    speedSlider.addListener(this);

    //Set the range of the sliders
    volSlider.setRange(0.0, 100.0, 1);
    speedSlider.setRange(0.0, 100.0, 1);
    //Style of sliders, make them vertical
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volSlider.setSliderStyle(juce::Slider::LinearHorizontal);

    //attach labels to the sliders
    addAndMakeVisible(speedSliderLabel);
    speedSliderLabel.setText("Speed", juce::dontSendNotification);
    speedSliderLabel.attachToComponent(&speedSlider, true);

    addAndMakeVisible(volSliderLabel);
    volSliderLabel.setText("Volume", juce::dontSendNotification);
    volSliderLabel.attachToComponent(&volSlider, true);

    //set default values for sliders
    volSlider.setValue(100.0);
    speedSlider.setValue(50.0);

    startTimer(500);

}

DeckGUI::~DeckGUI()
{
    stopTimer();
    player->transportSource.removeChangeListener(this);
}

void DeckGUI::paint(juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour(juce::Colours::grey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText("", getLocalBounds(),
        juce::Justification::centred, true);   // draw some placeholder text
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 8;

    playButton.setBounds(getWidth() / 4, 0, getWidth() / 2, rowH);
    stopButton.setBounds(getWidth() / 4, rowH, getWidth() / 2, rowH);

    volSlider.setBounds(getWidth() / 6, rowH * 7, getWidth()/2, rowH);
    speedSlider.setBounds(getWidth() / 6, rowH * 2, getWidth()/2, rowH);

    currentTimeLabel.setBounds(getWidth() / 2 + getWidth() / 6, rowH * 7, getWidth() / 2, rowH);
    waveformDisplay.setBounds(0, rowH * 5, getWidth(), rowH * 2);


}

void DeckGUI::buttonClicked(juce::Button* button) {

    if (button == &playButton) {
        player->start();

    }
    if (button == &stopButton) {
        DBG("stopButton was clicked");
        player->stop();

    }

}
void DeckGUI::sliderValueChanged(juce::Slider* slider) {

    if (slider == &volSlider) {
        player->setGain(slider->getValue() / 100.0);
    }

    if (slider == &speedSlider) {
        double sliderValue = slider->getValue();
        double speedValue;

        if (sliderValue <= 50) {
            speedValue = 0.5 + (sliderValue / 50) * 0.5;
        }
        else {
            speedValue = 1.0 + ((sliderValue - 50) / 50) * 1.0;
        }

        player->setSpeed(speedValue);
    }
       
}

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(player->getPositionRelative());

    // Get the current position of the song in seconds
    double currentPositionInSeconds = player->transportSource.getCurrentPosition();

    // Format the position and display it in the currentTimeLabel
    currentTimeLabel.setText(formatTime(currentPositionInSeconds), juce::dontSendNotification);
}

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray& files) {
    return true;
}
void DeckGUI::filesDropped(const juce::StringArray& files, int x, int y) {

    if (files.size() == 1) {
        player->loadURL(juce::URL{ juce::File{files[0]} });
        waveformDisplay.loadURL(juce::URL{ juce::File{files[0]} });
    }
}
//format time called in timerCallback to adjust posSlider
juce::String DeckGUI::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int remainingSeconds = totalSeconds % 60;
    return juce::String::formatted("%02d:%02d", minutes, remainingSeconds);
}

void DeckGUI::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &player->transportSource)
    {
        if (player->transportSource.hasStreamFinished())
        {
            // Reset the playhead position
            player->setPosition(0);
        }
    }
}



