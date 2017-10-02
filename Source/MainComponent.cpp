#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleSource.h"
#include "DrumPad.h"

class MainContentComponent 
		: public Component
{
public:
	MainContentComponent() :
		drumPad()
	{
		setSize( 500, 400 );

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
	}

private:
	AudioDeviceManager deviceManager;
	AudioSourcePlayer sourcePlayer;
	SampleSource sampleSource;
	DrumPad drumPad;

	//==========================================================================

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MainContentComponent )
};

Component* createMainContentComponent() { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED
