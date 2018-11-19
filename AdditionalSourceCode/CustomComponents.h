#pragma once

#include "JuceHeader.h"

#if USE_RAW_FRONTEND

struct GreyPanel : public Component
{
	void paint(Graphics& g) override;
};

class PageButtonLookAndFeel : public juce::LookAndFeel_V3
{
	void drawButtonText(Graphics& g, TextButton& b, bool /*isMouseOverButton*/, bool /*isButtonDown*/) override;
	void drawButtonBackground(Graphics& g, Button& b, const Colour& /*backgroundColour*/, bool isMouseOverButton, bool isButtonDown) override;
};

struct TitleLabel : public juce::Label
{
	TitleLabel(const juce::String& n, const juce::String& text, bool big);
};

struct SliderLabel : public juce::Label
{
	SliderLabel(const juce::String& n, const juce::String& text);
};

template <int ParameterIndex>
class StrippedSlider : public hise::HiSlider,
	public ControlledObject
{
public:

	StrippedSlider(MainController* mc, const String& name) :
		HiSlider(name),
		ControlledObject(mc)
	{
		raw::Pool pool(mc, true);
		flaf.setFilmstripImage(pool.loadImage("Strip.png"), 100);

		setName(name);
		setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
		setLookAndFeel(&flaf);
		setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	}

	~StrippedSlider()
	{
		connection = nullptr;
	}

	void connect(const String& id)
	{
		auto p = ProcessorHelpers::getFirstProcessorWithName(getMainController()->getMainSynthChain(), id);

		setup(p, ParameterIndex, Slider::getName());
		connection = new raw::UIConnection::Slider<ParameterIndex>(this, getMainController(), id);
		setLookAndFeel(&flaf);
	}

	bool tempoSyncMode = false;

	FilmstripLookAndFeel flaf;
	ScopedPointer<raw::UIConnection::Slider<ParameterIndex>> connection;
};

template <class ConnectionType> class BrightComboBox : public juce::ComboBox,
public ControlledObject
{
public:

	BrightComboBox(MainController* mc, const String& name, const String& processorId) :
		ControlledObject(mc),
		connection(this, mc, processorId)
	{
		setName(name);
		mc->skin(*this);

		setColour(HiseColourScheme::ComponentTextColourId, Colours::black);
		setColour(HiseColourScheme::ComponentBackgroundColour, Colours::transparentBlack);
		setColour(HiseColourScheme::ComponentOutlineColourId, Colours::transparentBlack);
		setColour(HiseColourScheme::ComponentFillTopColourId, Colour(0x11333333));
		setColour(HiseColourScheme::ComponentFillBottomColourId, Colour(0x20111111));

		connection.setData<ConnectionType>();
	}

	void setConnectionMode(raw::UIConnection::ComboBox::Mode newMode)
	{
		connection.setMode(newMode);
	}

private:

	raw::UIConnection::ComboBox connection;
};

class PowerButton : public juce::Button,
	public ControlledObject
{
public:

	PowerButton(MainController* mc, const String& name, const String& processorId);

	void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown);

	Path p;
	raw::UIConnection::Button connection;
};


#endif 