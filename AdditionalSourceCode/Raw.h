#pragma once

#include "JuceHeader.h"

#if USE_RAW_FRONTEND



class ColourMidiProcessor : public hise::HardcodedScriptProcessor
{
public:

	SET_PROCESSOR_NAME("Colour", "Colour");

		enum Parameters
	{
		ColourValue = 0,
		numParameters
	};

	ColourMidiProcessor(ModulatorSynth* owner, int index) :
		HardcodedScriptProcessor(owner->getMainController(), "Colour" + String(index+1), owner)
	{
		colour = Content.addKnob("colour", 0, 0);
		colour->setRange(-12, 12, 1);
	}

	void onNoteOn() override
	{
		Message.setTransposeAmount(-1 * (int)colour->getValue());
		Message.setCoarseDetune(colour->getValue());
	}

	hise::ScriptingApi::Content::ScriptSlider* colour;

	JUCE_DECLARE_WEAK_REFERENCEABLE(ColourMidiProcessor);
};

class VCSLData : public FrontendProcessor::RawDataBase
{
public:

	VCSLData(MainController* mc);;

	template <class FunctionType> void addToUserPreset(const String& storageId, const String& processorId)
	{
		auto p = ProcessorHelpers::getFirstProcessorWithName(getMainController()->getMainSynthChain(), processorId);
		storedData.add(new raw::Storage<FunctionType>(storageId, p));
	}

	void addParameter(const String& name, RawPluginParameter::Type t, const Array<RawPluginParameter::ProcessorConnection>& connections)
	{
		auto fp = dynamic_cast<FrontendProcessor*>(getMainController());

		auto* parameter = new RawPluginParameter(getMainController(), t, name, connections);

		fp->addParameter(parameter);
	}

	void restoreFromValueTree(const ValueTree& v) override
	{
		if (v.getNumChildren() != storedData.size())
		{
			jassertfalse;
			return;
		}

		for (int i = 0; i < v.getNumChildren(); i++)
		{
			storedData[i]->restoreFromValueTree(v.getChild(i));
		}
	}

	ValueTree exportAsValueTree() const
	{
		ValueTree v("Preset");

		for (auto s : storedData)
			v.addChild(s->exportAsValueTree(), -1, nullptr);

		return v;
	}

	Component* createEditor() override;

private:

	OwnedArray<raw::Data> storedData;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VCSLData);
	JUCE_DECLARE_WEAK_REFERENCEABLE(VCSLData);
};

class GreyPanel : public Component
{
	void paint(Graphics& g) override
	{
		g.setColour(Colour(0x1D000000));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);
	}
};

class PageButtonLookAndFeel : public juce::LookAndFeel_V3
{
	void drawButtonText(Graphics& g, TextButton& b, bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
	{
		g.setFont(GLOBAL_BOLD_FONT().withHeight(16.0f));
		g.setColour(Colour(0xFF444444).withAlpha(b.getToggleState() ? 1.0f : 0.4f));
		g.drawText(b.getButtonText(), b.getLocalBounds().toFloat(), Justification::centred);
	}

	void drawButtonBackground(Graphics& g, Button& b, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override
	{
		g.setColour(Colours::black.withAlpha(0.1f));

		if (isMouseOverButton)
			g.fillRoundedRectangle(b.getLocalBounds().toFloat(), 20.0f);

		if (isButtonDown)
			g.fillRoundedRectangle(b.getLocalBounds().toFloat(), 20.0f);
	}
};

class TitleLabel : public juce::Label
{
public:

	TitleLabel(const juce::String& n, const juce::String& text, bool big)
	{
		setName(n);
		setText(text, dontSendNotification);
		setEditable(false, false);
		setColour(Label::backgroundColourId, Colour(0x14000000));
		setColour(Label::textColourId, Colour(0xFF595959));
		setJustificationType(Justification::centred);

		setFont(GLOBAL_BOLD_FONT().withHeight(big ? 29.0f : 18.0f));
	}
};

class SliderLabel : public juce::Label
{
public:
	SliderLabel(const juce::String& n, const juce::String& text)
	{
		setName(n);
		setText(text, dontSendNotification);
		setEditable(false, false);
		setColour(Label::textColourId, Colours::black);
		setJustificationType(Justification::centred);

		setFont(GLOBAL_FONT());
	}
};

class VCSOInterface : public Component,
					  public ControlledObject,
					  public TextButton::Listener,
					  public MainController::SampleManager::PreloadListener
{
public:

	class SamplerTab : public Component,
					   public ControlledObject,
					   public ComboBox::Listener,
					   public SampleMap::Listener
	{
	public:

		static constexpr uint32 LeftColour = 0x22FF0000;
		static constexpr uint32 rightColour = 0x2200FFFF;

		SamplerTab(MainController* mc, int index);;

		~SamplerTab()
		{
			sampler->getSampleMap()->removeListener(this);
		}

		void sampleMapWasChanged(PoolReference newSampleMap) override
		{
			sampleMapSelector.setText(newSampleMap.getReferenceString(), dontSendNotification);
		}

		void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
		{
			if (sampleMapSelector.getText().isEmpty())
				return;

			auto ref = hise::raw::Pool(getMainController()).createSampleMapReference(sampleMapSelector.getText());

			auto f = [ref](Processor* p)
			{
				static_cast<ModulatorSampler*>(p)->loadSampleMap(ref);
				return SafeFunctionCall::OK;
			};

			hise::raw::TaskAfterSuspension::call(sampler, f);
		}

		String indexed(const String& name)
		{
			return name + String(index + 1);
		}

		int index;

		GreyPanel instrumentPanel;
		GreyPanel velocityPanel;
		
		TitleLabel instrumentTitle;
		TitleLabel veloTitle;

		SliderLabel colourLabel;
		SliderLabel releaseLabel;

		hise::raw::Reference<hise::ModulatorSampler> sampler;
		hise::raw::Reference<hise::VelocityModulator> velocityMod;
		hise::raw::Reference<ColourMidiProcessor> colorScript;



		hise::TableEditor velocityEditor;

		juce::ComboBox sampleMapSelector;

		hise::FilmstripLookAndFeel flaf;
		juce::Slider releaseSlider;
		juce::Slider colourSlider;
		hise::raw::UIConnection::Slider colourConnection;
		hise::raw::UIConnection::Slider releaseConnection;
	};

	void addAndSetName(Component& parent, Component& c, const String& name)
	{
		parent.addAndMakeVisible(c);
		c.setName(name);
	}

	VCSOInterface(VCSLData* data);

	void paint(Graphics& g) override;

	void resized() override;
	void refreshKeyboardColours();

	void preloadStateChanged(bool isPreloading) override
	{
		if (!isPreloading)
			refreshKeyboardColours();
	}

	void buttonClicked(Button* b) override;

	void showPage(Button* b);
private:

	bool isMapped(int noteNumber, int samplerIndex) const;

	juce::ImageComponent hiseLogo;
	juce::ImageComponent vcslLogo;

	TitleLabel mainTitle;

	PageButtonLookAndFeel plaf;

	juce::TextButton editButton;
	juce::TextButton browseButton;
	juce::TextButton settingsButton;

	juce::Component editPage;


	SamplerTab tab1;
	SamplerTab tab2;

	

	juce::Component browsePage;
	GreyPanel presetBrowserBg;
	hise::PresetBrowser presetBrowser;


	juce::Component settingsPage;
	GreyPanel settingsBg;
	hise::FloatingTile audioSettings;
	hise::FloatingTile midiSources;
	TitleLabel audioLabel;
	TitleLabel midiLabel;

	hise::FloatingTile keyboard;
};

#endif