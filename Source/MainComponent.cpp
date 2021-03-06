#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleSource.h"
#include "DrumPad.h"

class MainContentComponent 
		: public Component
{
public:
	MainContentComponent()
	{
		setSize( 480, 320 );

		/*
		for( int i = 0; i < k_numDrumPads; ++i )
		{
			addAndMakeVisible( drumPads[ i ] );
			mixerSource.addInputSource( drumPads[ i ].getOutputAudioSource(), false );
		}

		sourcePlayer.setSource( &mixerSource );
		*/

		addAndMakeVisible( drumPad );
		sourcePlayer.setSource( drumPad.getOutputAudioSource() );

		deviceManager.addAudioCallback( &sourcePlayer );
		deviceManager.initialiseWithDefaultDevices( 0, 1 );
	}

	~MainContentComponent()
	{
		deviceManager.removeAudioCallback( &sourcePlayer );
	}

	void resized() override
	{
		drumPad.setBounds( 0, 0, getWidth(), getHeight() );

		/*
		for( int row = 0; row < k_numRows; ++row )
		{
			for( int col = 0; col < k_numCols; ++col )
			{
				int index = row * k_numCols + col;
				int width = getWidth() / k_numCols;
				int height = getHeight() / k_numRows;

				drumPads[ index ].setBounds( col * width,
											 row * height,
											 width,
											 height );
			}
		}
		*/
	}

private:
	const int k_numRows = 2;
	const int k_numCols = 2;
	const int k_numDrumPads = k_numRows * k_numCols;

	AudioDeviceManager deviceManager;
	AudioSourcePlayer sourcePlayer;
	MixerAudioSource mixerSource;
	//DrumPad drumPads[ 4 ];
	DrumPad drumPad;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MainContentComponent )
};

Component* createMainContentComponent() { return new MainContentComponent(); }
