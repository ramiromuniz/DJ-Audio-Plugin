#include "MainComponent.h"
#include "DJAudioPlayer.h"
#include "DeckGUI.h"




//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize(800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }



    // Load the playlist from the saved file

    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);


    addAndMakeVisible(playlistComponents);


    formatManager.registerBasicFormats();

    // Set up the crossfader slider
    addAndMakeVisible(crossFaderSlider);
    crossFaderSlider.setRange(0.0, 100.0, 1.0);
    crossFaderSlider.setValue(0.0);
    crossFaderSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    addAndMakeVisible(crossFaderLabel);
    crossFaderLabel.setText("Crossfader", juce::dontSendNotification);
    crossFaderLabel.attachToComponent(&crossFaderSlider, true);
    crossFaderSlider.addListener(this);

    //function that calls loadPlaylist
    initialise();
    
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);

}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);

}



void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()

    player1.releaseResources();
    player2.releaseResources();

    mixerSource.releaseResources();

}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!

}

void MainComponent::resized()
{

    deckGUI1.setBounds(0, 0, getWidth() / 2, getHeight() * 5 / 10);
    deckGUI2.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight() * 5 / 10);
    crossFaderSlider.setBounds(getWidth()/3, getHeight() * 5 / 10, getWidth()/3, getHeight() / 10);
    playlistComponents.setBounds(0, getHeight() * 6 / 10, getWidth(), getHeight() * 4 / 10);
}

void MainComponent::buttonClicked(juce::Button* button) {

}

//crossfader slider implementation
void MainComponent::sliderValueChanged(juce::Slider* slider) {

    if (slider == &crossFaderSlider) {

        // Get the current value of the crossfader slider
        float sliderValue = static_cast<float> (crossFaderSlider.getValue());

        // Map the slider value to a range between 0 and 1
        float sliderNormalized = sliderValue / 100.0f;

        // Calculate the volume for the left deck (0% to 100%)
        float leftVolume = 1.0f - sliderNormalized;

        // Calculate the volume for the right deck (0% to 100%)
        float rightVolume = sliderNormalized;

        // Set the volumes for both decks using linear interpolation
        deckGUI1.player->setGain(leftVolume);
        deckGUI2.player->setGain(rightVolume);
        
    }
}

void MainComponent::initialise()
{
    // Load the playlist from the file
    playlistComponents.loadPlaylist(playlistComponents.playlistFile);

}
