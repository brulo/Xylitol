#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class NonStreamingSamplerSource 
	: public AudioSource,
	  public Component,
	  private ButtonListener
{
public:
    NonStreamingSamplerSource() :
		position( -1 ),
		isLooping( false )
    {
        addAndMakeVisible( openButton );
        openButton.setButtonText( "Open..." );
        openButton.addListener( this );

        addAndMakeVisible( playButton );
        playButton.setButtonText( "Play" );
        playButton.addListener( this );

		addAndMakeVisible( reverseButton );
		reverseButton.setButtonText( "Reverse" );
		reverseButton.addListener( this );

        formatManager.registerBasicFormats();
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
		if( position < 0 || fileBuffer.getNumSamples() < 1 )
		{
			bufferToFill.clearActiveBufferRegion();
			return;
		}

        const int numInputChannels = fileBuffer.getNumChannels();
        const int numOutputChannels = bufferToFill.buffer->getNumChannels();

        int outputSamplesRemaining = bufferToFill.numSamples;
        int outputSamplesOffset = bufferToFill.startSample;

        while (outputSamplesRemaining > 0)
        {
            int bufferSamplesRemaining = fileBuffer.getNumSamples() - position;
            int samplesThisTime = jmin (outputSamplesRemaining, bufferSamplesRemaining);

            for (int channel = 0; channel < numOutputChannels; ++channel)
            {
                bufferToFill.buffer->copyFrom (channel,
                                               outputSamplesOffset,
                                               fileBuffer,
                                               channel % numInputChannels,
                                               position,
                                               samplesThisTime);
            }

            outputSamplesRemaining -= samplesThisTime;
            outputSamplesOffset += samplesThisTime;
            position += samplesThisTime;

            if( isLooping && position == fileBuffer.getNumSamples() )
                position = 0;
        }
    }

	void prepareToPlay( int /*samplesPerBlockExpected*/, double /*sampleRate*/ ) override
	{
	}

    void releaseResources() override
    {
        fileBuffer.setSize (0, 0);
    }

    void resized() override
    {
        openButton.setBounds( 10, 10, getWidth() - 20, 20 );
		playButton.setBounds( 10, 40, getWidth() - 20, 20 );
		reverseButton.setBounds( 10, 70, 50, 50 );
    }

    void buttonClicked (Button* button) override
    {
        if (button == &openButton)      openButtonClicked();
        if (button == &playButton)      playButtonClicked();
		if( button == &reverseButton )  reverseButtonClicked();
    }

private:
	void reverseButtonClicked()
	{
		fileBuffer.reverse( 0, fileBuffer.getNumSamples() );
		isReversed = !isReversed;
		reverseButton.setToggleState( isReversed, NotificationType::dontSendNotification );
	}


    void openButtonClicked()
    {
        FileChooser chooser ("Select a Wave file shorter than 2 seconds to play...",
                             File::nonexistent,
                             "*.wav");

        if (chooser.browseForFileToOpen())
        {
            const File file (chooser.getResult());
            ScopedPointer<AudioFormatReader> reader (formatManager.createReaderFor (file));

            if (reader != nullptr)
            {
                const double duration = reader->lengthInSamples / reader->sampleRate;

                if( duration < 10 )
                {
                    fileBuffer.setSize (reader->numChannels, reader->lengthInSamples);
                    reader->read (&fileBuffer,
                                  0,
                                  reader->lengthInSamples,
                                  0,
                                  true,
                                  true);
                    position = 0;

					if( isReversed )
					{
						fileBuffer.reverse( 0, fileBuffer.getNumSamples() );
					}
                }
                else
                {
                    // handle the error that the file is 2 seconds or longer..
                }
            }
        }
    }
	
	void playButtonClicked()
	{
		position = 0;
	}

    //==========================================================================
    TextButton openButton;
    TextButton playButton;
	ToggleButton reverseButton;

    AudioFormatManager formatManager;
    AudioSampleBuffer fileBuffer;
    int position;
	bool isLooping;
	bool isReversed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NonStreamingSamplerSource)
};
