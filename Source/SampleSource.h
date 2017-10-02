#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SampleSource
	: public AudioSource,
	  public Component,
	  public ChangeListener,
	  public Button::Listener
{
public:
    SampleSource()
		: state( Stopped )
    {
		addAndMakeVisible( &openButton );
		openButton.setButtonText( "Open..." );
		openButton.addListener( this );

		addAndMakeVisible( &playButton );
		playButton.setButtonText( "Play" );
		playButton.addListener( this );
		playButton.setColour( TextButton::buttonColourId, Colours::green );
		playButton.setEnabled( false );

		addAndMakeVisible( &stopButton );
		stopButton.setButtonText( "Stop" );
		stopButton.addListener( this );
		stopButton.setColour( TextButton::buttonColourId, Colours::red );
		stopButton.setEnabled( false );

		formatManager.registerBasicFormats();

		transportSource.addChangeListener( this );
    }

    ~SampleSource()
    {
    }

	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Stopping
	};
	
	void prepareToPlay( int samplesPerBlockExpected, double sampleRate ) override
	{
		transportSource.prepareToPlay( samplesPerBlockExpected, sampleRate );
	}
	
	void getNextAudioBlock( const AudioSourceChannelInfo& bufferToFill ) override
	{
		if( readerSource == nullptr )
		{
			bufferToFill.clearActiveBufferRegion();
			return;
		}

		transportSource.getNextAudioBlock( bufferToFill );
	}
	
	void releaseResources() override
	{
		transportSource.releaseResources();
	}

	void buttonClicked( Button* button ) override
	{
		if( button == &openButton )  openButtonClicked();
		if( button == &playButton )  playButtonClicked();
		if( button == &stopButton )  stopButtonClicked();
	}
	void playButtonClicked()
	{
		changeState( Starting );
	}

	void stopButtonClicked()
	{
		changeState( Stopping );
	}
	void openButtonClicked()
	{
		FileChooser chooser( "Select a Wave file to play...",
							 File::nonexistent,
							 "*.wav" );

		if( chooser.browseForFileToOpen() )
		{
			File file( chooser.getResult() );
			AudioFormatReader* reader = formatManager.createReaderFor( file );

			if( reader != nullptr )
			{
				ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource( reader, true );
				transportSource.setSource( newSource, 0, nullptr, reader->sampleRate );
				playButton.setEnabled( true );
				readerSource = newSource.release();
			}
		}
	}

	void changeState( SampleSource::TransportState newState )
	{
		if( state != newState )
		{
			state = newState;

			switch( state )
			{
				case Stopped:
					stopButton.setEnabled( false );
					playButton.setEnabled( true );
					transportSource.setPosition( 0.0 );
					break;

				case Starting:
					playButton.setEnabled( false );
					transportSource.start();
					break;

				case Playing:
					stopButton.setEnabled( true );
					break;

				case Stopping:
					transportSource.stop();
					break;
			}
		}
	}

	void changeListenerCallback( ChangeBroadcaster* source ) override
	{
		if( source == &transportSource )
		{
			if( transportSource.isPlaying() )
				changeState( Playing );
			else
				changeState( Stopped );
		}
	}

    void resized() override
    {
		openButton.setBounds( 10, 10, getWidth() - 20, 20 );
		playButton.setBounds( 10, 40, getWidth() - 20, 20 );
		stopButton.setBounds( 10, 70, getWidth() - 20, 20 );
    }

private:	
	TextButton openButton;
	TextButton playButton;
	TextButton stopButton;

	AudioFormatManager formatManager;
	ScopedPointer<AudioFormatReaderSource> readerSource;
	AudioTransportSource transportSource;
	TransportState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleSource)
};
