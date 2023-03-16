#include <JuceHeader.h>
#include "PlaylistComponent.h"


//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1, DeckGUI* _deckGUI2,
    juce::AudioFormatManager& formatManagerToUse,
    juce::AudioThumbnailCache& cacheToUse)
    : deckGUI1(_deckGUI1),
    deckGUI2(_deckGUI2)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.


    addAndMakeVisible(addButton);
    addAndMakeVisible(leftDeck);
    addAndMakeVisible(rightDeck);
    addAndMakeVisible(removeButton);


    leftDeck.addListener(this);
    rightDeck.addListener(this);
    addButton.addListener(this);
    removeButton.addListener(this);


    tableComponent.getHeader().addColumn("Track title", 1, 300);
    tableComponent.getHeader().addColumn("Artist", 2, 300);
    tableComponent.getHeader().addColumn("Duration", 3, 100);
    tableComponent.getHeader().addColumn("BPM", 4, 50);


    //search box
    addAndMakeVisible(searchBox);
    searchBox.addListener(this);
    searchBox.setColour(juce::TextEditor::textColourId, juce::Colours::grey);
    searchBox.setText("Search for songs...", juce::dontSendNotification);



    tableComponent.setModel(this);
    addAndMakeVisible(tableComponent);
    tableComponent.addMouseListener(this, true);

    playlistFile = juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("playlist.xml");

}

PlaylistComponent::~PlaylistComponent()
{
    // Save the tracks vector to the playlist file
    savePlaylist(playlistFile);
}

void PlaylistComponent::paint(juce::Graphics& g)
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

    if (tracks.empty())
    {
        g.drawText("Add tracks to start playing", getWidth() / 2-100, getHeight() * 8 / 10, getWidth() / 2, getHeight() / 10,
            juce::Justification::left, true);
    }
    else {
        g.drawText("Load them to a deck to start playing", getWidth() / 2 - 100, getHeight() * 8 / 10, getWidth() / 2, getHeight() / 10,
            juce::Justification::left, true);
    }
}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

    tableComponent.setBounds(0, 0, getWidth(), getHeight() * 8 / 10);

    //Add Tracks Buttons
    int buttonWidth = getWidth();
    int buttonHeight = getHeight() / 10;
    addButton.setBounds(getWidth()/ 3, getHeight() * 9 / 10, getWidth()/3, buttonHeight);
    leftDeck.setBounds(0, getHeight() * 9 / 10, getWidth()/6, buttonHeight);
    rightDeck.setBounds(getWidth()/ 6, getHeight() * 9 / 10, getWidth()/6, buttonHeight);
    removeButton.setBounds(getWidth() * 2 / 3, getHeight() * 9 / 10, getWidth() / 6, buttonHeight);

    searchBox.setBounds(getWidth() * 5 / 6, getHeight() * 9 / 10, getWidth() / 6, getHeight() / 10);

}

int PlaylistComponent::getNumRows()
{
    return isFiltered ? filteredTracks.size() : tracks.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected) {
        g.fillAll(juce::Colours::lightblue);
    }
    else {
        g.fillAll(juce::Colours::darkgrey);
    }
}

void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    //add columns and rows according to the lenght and content of the tracks vector containing the files
        auto& track = isFiltered ? filteredTracks[rowNumber] : tracks[rowNumber];

        if (columnId == 1) // Track title column
        {
            g.drawText(track.filename,
                2, 0,
                width - 4, height,
                juce::Justification::centredLeft,
                true);
        }
        else if (columnId == 3) // Duration column
        {
            g.drawText(formatDuration(track.duration),
                2, 0,
                width - 4, height,
                juce::Justification::centredLeft,
                true);
        }
        else if (columnId == 4) // BPM column
        {
            g.drawText(juce::String(track.bpm),
                2, 0,
                width - 4, height,
                juce::Justification::centredLeft,
                true);
        }
        else if (columnId == 2) // artist column
        {
            g.drawText(track.artist,
                2, 0,
                width - 4, height,
                juce::Justification::centredLeft,
                true);
        }
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    Component* existingComponentToUpdate) {
    return existingComponentToUpdate;
}


//buttonClicked listener
void PlaylistComponent::buttonClicked(juce::Button* button)
{

    if (button == &addButton) {
        auto fileChooserFlags = juce::FileBrowserComponent::canSelectFiles;

        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
            {
                juce::File chosenFile = chooser.getResult();

                // Check if file is already in playlist
                bool duplicateFound = false;
                for (auto& track : tracks)
                {
                    if (chosenFile.getFileName().toStdString() == track.filename)
                    {
                        duplicateFound = true;
                        break;
                    }
                }

                if (duplicateFound)
                {
                    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                        "Duplicate song",
                        "The selected song is already in the playlist.");
                }
                else
                {
                    // Create a new TrackInfo object for the selected file
                    TrackInfo newTrack;
                    newTrack.filename = chosenFile.getFileName().toStdString();
                    // Set the file path to the chosen file
                    newTrack.filePath = chosenFile;

                    // Get the file duration and BPM
                    juce::AudioFormatManager formatManager;
                    formatManager.registerBasicFormats();
                    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(chosenFile));
                    if (reader.get() != nullptr)
                    {
                        newTrack.duration = reader->lengthInSamples / static_cast<double>(reader->sampleRate);
                        juce::String bpmString = reader->metadataValues.getValue("BPM", "");
                        if (!bpmString.isEmpty())
                            newTrack.bpm = bpmString.getDoubleValue();

                        juce::String artistString = reader->metadataValues.getValue("ARTIST", "");
                        if (!artistString.isEmpty())
                            newTrack.artist = artistString.toStdString();
                    }

                    // Add the new track to the tracks vector
                    tracks.push_back(newTrack);

                    // Update the table
                    tableComponent.updateContent();
                    repaint(); // Add this to update the UI

                }
            });
    }

    //Handle the load buttons to deck
    if (button == &leftDeck)
    {
        DBG("leftDeck button clicked");
        // Load the selected track to the left deck
        if (selectedFilePath.existsAsFile())
        {
            // Use the selectedFilePath variable to load the track to the left deck
            deckGUI1->player->loadURL(juce::URL{ selectedFilePath });
            deckGUI1->waveformDisplay.loadURL(juce::URL{ selectedFilePath });
        }
    }
    else if (button == &rightDeck)
    {
        DBG("rightDeck button clicked");

        // Load the selected track to the right deck
        if (selectedFilePath.existsAsFile())
        {
            // Use the selectedFilePath variable to load the track to the right deck
            deckGUI2->player->loadURL(juce::URL{ selectedFilePath });
            deckGUI2->waveformDisplay.loadURL(juce::URL{ selectedFilePath });
        }
    }

    // Handle the removeTrackButton click
    if (button == &removeButton)
    {

        // Remove the selected track from the tracks vector
        int selectedRow = tableComponent.getSelectedRow();
        DBG("Selected row: " << selectedRow); // Debug statement

        if (selectedRow >= 0 && selectedRow < tracks.size())
        {
            tracks.erase(tracks.begin() + selectedRow);
            tableComponent.updateContent();
            tableComponent.repaint();

        }
    }
}


//function to convert the duration in seconds to mm:ss format
juce::String PlaylistComponent::formatDuration(double seconds)
{
    int minutes = static_cast<int>(seconds) / 60;
    int remainingSeconds = static_cast<int>(seconds) % 60;

    return juce::String::formatted("%02d:%02d", minutes, remainingSeconds);
}


//implementation of the search box to filter tracks in the playlist
//filter tracks algorithm
void PlaylistComponent::filterTracks(const juce::String& searchTerm)
{

    if (searchTerm.isNotEmpty())
    {
        // If the playlist is not currently filtered, copy the original tracks to the filtered tracks
        if (!isFiltered)
            filteredTracks = tracks;

        // Filter the tracks based on the search term
        filteredTracks.erase(std::remove_if(filteredTracks.begin(), filteredTracks.end(),
            [&](const TrackInfo& track) {
                // Check if the track's filename or artist starts with the search term
                return !(juce::String(track.filename).startsWithIgnoreCase(searchTerm)
                    || juce::String(track.artist).startsWithIgnoreCase(searchTerm));
            }), filteredTracks.end());

        isFiltered = true;
    }
    else
    {
        // If the search term is empty, the playlist is not filtered anymore
        isFiltered = false;
        filteredTracks.clear();
    }

    // update the tableComponent to display the filtered (or complete) playlist
    tableComponent.updateContent();
}

//when user presses the enter key in the serarchbox this function is called
void PlaylistComponent::textEditorReturnKeyPressed(juce::TextEditor& editor)
{
    if (&editor == &searchBox)
    {
        // get the search term
        juce::String searchTerm = searchBox.getText();

        if (searchTerm.isEmpty()) // if search term is empty, display the complete playlist
        {
            isFiltered = false;

        }
        else // otherwise, filter the tracks
        {
            filterTracks(searchTerm);

        }

        // update the tableComponent to display the filtered (or complete) playlist
        tableComponent.updateContent();
    }
}

//save playlist before closing the app in the users computer
void PlaylistComponent::savePlaylist(const juce::File& file)
{

    // Clear the contents of the file before saving
    file.deleteFile();
    file.create();

    // Create an output stream for the file
    juce::FileOutputStream outputStream(file);

    // Write the tracks vector to the output stream
    for (const auto& track : tracks)
    {
        outputStream.writeText(track.filename + "|" + juce::String(track.duration) + "|" +
            juce::String(track.bpm) + "|" + track.artist + "|" + track.filePath.getFullPathName() + "\n", false, false, "");
    }
}

//Load playlist when the app is launched
void PlaylistComponent::loadPlaylist(const juce::File& file)
{

    // Create an input stream for the file
    juce::FileInputStream inputStream(file);

    // Clear the current playlist
    tracks.clear();
    filteredTracks.clear();
    isFiltered = false;

    // Read the tracks from the input stream
    juce::String fileContent = inputStream.readEntireStreamAsString();
    juce::StringArray trackLines;
    trackLines.addLines(fileContent);

    for (const auto& line : trackLines)
    {
       // Parse the track information from the line
        juce::StringArray tokens;
        tokens.addTokens(line, "|", "");

        if (tokens.size() == 5)
        {
            TrackInfo track;
            track.filename = tokens[0];
            track.duration = tokens[1].getDoubleValue();
            track.bpm = tokens[2].getDoubleValue();
            track.artist = tokens[3];
            track.filePath = juce::File(tokens[4]);

            // Add the track to the playlist
            tracks.push_back(track);
        }
    }

    // Update the table component to display the loaded playlist
    tableComponent.updateContent();
    repaint();

}

void PlaylistComponent::mouseDown(const juce::MouseEvent& event)
{
    // Get the selected row number from the tableComponent
    const int selectedRow = tableComponent.getSelectedRow();

    // Check if a row is actually selected
    if (selectedRow >= 0)
    {
          // Retrieve the file path of the selected track
        juce::String filePathString = tracks[selectedRow].filePath.getFullPathName();

        // Create a juce::File object from the file path string
        selectedFilePath = juce::File::createFileWithoutCheckingPath(filePathString);
    }
}

//searchbox placeholder
void PlaylistComponent::textEditorFocusGained(juce::TextEditor& editor)
{
    if (searchBox.getText() == "Search for songs...")
    {
        searchBox.setColour(juce::TextEditor::textColourId, juce::Colours::black);
        searchBox.setText("", juce::dontSendNotification);
    }
}

void PlaylistComponent::textEditorFocusLost(juce::TextEditor& editor)
{
    if (editor.getText().isEmpty())
    {
        editor.setColour(juce::TextEditor::textColourId, juce::Colours::grey);
        editor.setText("Search for songs...", juce::dontSendNotification);
    }
}







