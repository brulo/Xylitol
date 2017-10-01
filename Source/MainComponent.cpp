#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleSource.h"

class MainContentComponent 
		: public Component,
		  public Slider::Listener
{
public:
	MainContentComponent()
		: sampleSource(),
		filter( &sampleSource, false )

	{
		setSize( 500, 400 );

		addAndMakeVisible( sampleSource );

		filterFreqSlider.setTextBoxStyle( Slider::TextBoxBelow, true, 100, 20 );
		filterFreqSlider.setSliderStyle( Slider::SliderStyle::Rotary );
		filterFreqSlider.setName( "Filter Cutoff" );
		filterFreqSlider.setRange( 20, 20000 );
		filterFreqSlider.addListener( this );
		addAndMakeVisible( filterFreqSlider );

		sourcePlayer.setSource( &filter );

		deviceManager.addAudioCallback( &sourcePlayer );

		deviceManager.initialiseWithDefaultDevices( 0, 1 );

		m_sampleRate = deviceManager.getCurrentAudioDevice()->getCurrentSampleRate();
	}

	~MainContentComponent()
	{
		deviceManager.removeAudioCallback( &sourcePlayer );
	}

	void resized() override
	{
		sampleSource.setBounds( 0, 0, getWidth(), getHeight() );
		filterFreqSlider.setBounds( getWidth() * 0.5, getHeight() - 150, 100, 100 );
	}

	void sliderValueChanged( Slider* slider ) override
	{
		if( slider == &filterFreqSlider )
		{
			filter.setCoefficients( IIRCoefficients::makeLowPass( m_sampleRate, slider->getValue() ) );
		}
	}

private:
	AudioDeviceManager deviceManager;
	AudioSourcePlayer sourcePlayer;
	SampleSource sampleSource;
	IIRFilterAudioSource filter;
	Slider filterFreqSlider;
	double m_sampleRate;

	//==========================================================================

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MainContentComponent )
};

Component* createMainContentComponent() { return new MainContentComponent(); }

#endif  // MAINCOMPONENT_H_INCLUDED
