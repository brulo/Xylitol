#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DrumPad
	: public Component
{
public:
	DrumPad() :
		sampleSource()
    {
		addAndMakeVisible( sampleSource );
    }

    ~DrumPad()
    {
    }

	AudioSource* getOutputAudioSource()
	{
		return &sampleSource;
	}

    void resized() override
    {
		sampleSource.setBounds( 0, 0, getWidth(), getHeight() );
    }

private:
	SampleSource sampleSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumPad)
};
