#pragma once

#include <JuceHeader.h>
#include<vector>
#include<string>
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include "DeckGUI.h"

//here we will store the information of a track when its loaded to keep in a vector
struct TrackInfo
{
    juce::String filename;
    double duration = 0.0;
    double bpm = 0.0;
    juce::String artist;
    juce::File filePath; // Full file path
};

class PlaylistComponent : public juce::Component,
    public juce::TableListBoxModel,
    public juce::Button::Listener,
    public juce::TextEditor::Listener,
    public juce::MouseListener
{
public:
    PlaylistComponent(DeckGUI* deckGUI1, DeckGUI* deckGUI2,
    juce::AudioFormatManager& formatManagerToUse,
    juce::AudioThumbnailCache& cacheToUse);


    ~PlaylistComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    int getNumRows() override;

    void paintRowBackground(juce::Graphics& g,
        int rowNumber,
        int width,
        int height,
        bool rowIsSelected) override;

    void paintCell(juce::Graphics& g,
        int rowNumber,
        int columnId,
        int width,
        int height,
        bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber,
        int columnId,
        bool isRowSelected,
        Component* existingComponentToUpdate) override;

    void buttonClicked(juce::Button* button) override;

    juce::String formatDuration(double seconds);
    void filterTracks(const juce::String& searchTerm);
    void textEditorReturnKeyPressed(juce::TextEditor& editor);

    void savePlaylist(const juce::File& file);
    void loadPlaylist(const juce::File& file);

    void mouseDown(const juce::MouseEvent& event);

    void textEditorFocusGained(juce::TextEditor& editor);
    void textEditorFocusLost(juce::TextEditor& editor) override;


    juce::File playlistFile; // store the file where the playlist will be saved


private:
    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;

    std::vector<TrackInfo> tracks; // the vector of track information

    juce::TextButton addButton{ "ADD TRACKS" };
    juce::TextButton leftDeck{ "Load To Left Deck" };
    juce::TextButton rightDeck{ "Load To Right Deck" };
    juce::TextButton removeButton{ "Remove From Playlist" };

    juce::File selectedFilePath;

    juce::File loadedFile;


    juce::FileChooser fChooser{ "Select a file..." };

    juce::TableListBox tableComponent;


    std::vector<std::string> trackTitles;

    std::vector<juce::String> trackDurations;
    std::vector<std::string> trackBPMs;


    std::vector<TrackInfo> filteredTracks;

    juce::TextEditor searchBox; // search box


    bool isFiltered = false;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};
