#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SampleSource
	: public AudioSource,
	public Component,
	private ButtonListener,
	private ChangeListener
{
public:
	SampleSource() :
		position( -1 ),
		isLooping( false ),
		thumbnailCache( 1 ),
		thumbnail( 512, formatManager, thumbnailCache )
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

		thumbnail.addChangeListener( this );

		formatManager.registerBasicFormats();
	}

	void getNextAudioBlock( const AudioSourceChannelInfo& bufferToFill ) override
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

		while( outputSamplesRemaining > 0 )
		{
			int bufferSamplesRemaining = fileBuffer.getNumSamples() - position;
			int samplesThisTime = jmin( outputSamplesRemaining, bufferSamplesRemaining );

			for( int channel = 0; channel < numOutputChannels; ++channel )
			{
				bufferToFill.buffer->copyFrom( channel,
											   outputSamplesOffset,
											   fileBuffer,
											   channel % numInputChannels,
											   position,
											   samplesThisTime );
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
		fileBuffer.setSize( 0, 0 );
	}

	void resized() override
	{
		openButton.setBounds( 10, 10, getWidth() - 20, 20 );
		playButton.setBounds( 10, 40, getWidth() - 20, 20 );
		reverseButton.setBounds( 10, 70, getWidth() - 20, 20 );
	}

	void paint( Graphics& g ) override
	{
		const Rectangle<int> thumbnailBounds( 10, 100, getWidth() - 20, getHeight() - 120 );

		if( thumbnail.getNumChannels() == 0 )
			paintIfNoFileLoaded( g, thumbnailBounds );
		else
			paintIfFileLoaded( g, thumbnailBounds );
	}

	void paintIfNoFileLoaded( Graphics& g, const Rectangle<int>& thumbnailBounds )
	{
		g.setColour( Colours::darkgrey );
		g.fillRect( thumbnailBounds );
		g.setColour( Colours::white );
		g.drawFittedText( "No File Loaded", thumbnailBounds, Justification::centred, 1.0f );
	}

	void paintIfFileLoaded( Graphics& g, const Rectangle<int>& thumbnailBounds )
	{
		g.setColour( Colours::white );
		g.fillRect( thumbnailBounds );

		g.setColour( Colours::red );                                     // [8]

		thumbnail.drawChannels( g,                                      // [9]
								thumbnailBounds,
								0.0,                                    // start time
								thumbnail.getTotalLength(),             // end time
								1.0f );                                  // vertical zoom
	}

	void changeListenerCallback( ChangeBroadcaster* source ) override
	{
		if( source == &thumbnail )       repaint();
	}

	void buttonClicked( Button* button ) override
	{
		if( button == &openButton )      openButtonClicked();
		if( button == &playButton )      playButtonClicked();
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
		FileChooser chooser( "Select a Wave file shorter than 2 seconds to play...",
							 File::nonexistent,
							 "*.wav" );

		if( chooser.browseForFileToOpen() )
		{
			const File file( chooser.getResult() );
			ScopedPointer<AudioFormatReader> reader( formatManager.createReaderFor( file ) );

			if( reader != nullptr )
			{
				const double duration = reader->lengthInSamples / reader->sampleRate;

				if( duration < 10 )
				{
					fileBuffer.setSize( reader->numChannels, reader->lengthInSamples );
					reader->read( &fileBuffer,
								  0,
								  reader->lengthInSamples,
								  0,
								  true,
								  true );
					position = 0;

					if( isReversed )
					{
						fileBuffer.reverse( 0, fileBuffer.getNumSamples() );
					}

					thumbnail.setSource( new FileInputSource( file ) );
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

	TextButton openButton;
	TextButton playButton;
	ToggleButton reverseButton;

	AudioFormatManager formatManager;
	AudioSampleBuffer fileBuffer;

	AudioThumbnailCache thumbnailCache;
	AudioThumbnail thumbnail;

	int position;
	bool isLooping;
	bool isReversed = false;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SampleSource )
};
