#include <JuceHeader.h>
#include "WaveformDisplay.h"
#include <juce_core/juce_core.h>

//==============================================================================
WaveformDisplay::WaveformDisplay(juce::AudioFormatManager& formatManagerToUse,
    juce::AudioThumbnailCache& cacheToUse,
    DJAudioPlayer* playerToUse) :
    audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    position(0),
    player(playerToUse),
    playedWaveformColor(juce::Colours::blue),
    unplayedWaveformColor(juce::Colours::lightgrey)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);
    g.drawRect(getLocalBounds(), 1);   // draw an outline around the component

    g.setColour(juce::Colours::lightblue);

    if (fileLoaded) {
        audioThumb.drawChannel(g,
            getLocalBounds(),
            0,
            audioThumb.getTotalLength(),
            0,
            1.0f);

        // Draw a filled rectangle for the played part of the waveform
        g.setColour(juce::Colours::blue.withAlpha(0.5f));
        g.fillRect(0, 0, position * getWidth(), getHeight());

        // Draw a blue line for the playhead
        g.setColour(juce::Colours::lightblue);
        g.drawLine(position * getWidth(), 0, position * getWidth(), getHeight(), 2.0f);
    }
    else {
        g.setFont(20.0f);
        g.drawText("File not loaded...", getLocalBounds(),
            juce::Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::loadURL(juce::URL audioURL) {

    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
    if (fileLoaded) {
        DBG("wfd: loaded");
        repaint();
    }
    else {
        DBG("wfd: not loaded");
    }

}


void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source) {

    DBG("wfd: changed received");
    repaint();

}

void WaveformDisplay::setPositionRelative(double pos)
{

    if (pos != position && pos > 0) {
        position = pos;
        repaint();
    }
}

void WaveformDisplay::mouseDown(const juce::MouseEvent& event)
{
    updatePlayheadPositionFromMouseEvent(event);
}

void WaveformDisplay::mouseDrag(const juce::MouseEvent& event)
{
    updatePlayheadPositionFromMouseEvent(event);
}

void WaveformDisplay::updatePlayheadPositionFromMouseEvent(const juce::MouseEvent& event)
{
    if (audioThumb.getTotalLength() > 0)
    {
        double relativeX = static_cast<double>(event.x) / static_cast<double>(getWidth());
        double newPosition = audioThumb.getTotalLength() * relativeX;
        player->setPosition(newPosition);
    }
}
