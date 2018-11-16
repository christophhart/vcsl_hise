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

	void addParameter(AudioProcessorParameter* p)
	{
		auto fp = dynamic_cast<FrontendProcessor*>(getMainController());

		fp->addParameter(p);
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

	OwnedArray<raw::GenericStorage> storedData;

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

	void drawButtonBackground(Graphics& g, Button& b, const Colour& /*backgroundColour*/, bool isMouseOverButton, bool isButtonDown) override
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

template <int ParameterIndex> 
class StrippedSlider : public hise::HiSlider,
					   public ControlledObject
{
public:

	StrippedSlider(MainController* mc, const String& name):
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

#if 0
	void setTempoMode(bool useTempoMode)
	{
		if (useTempoMode != tempoSyncMode)
		{
			tempoSyncMode = useTempoMode;
			setRange(0, TempoSyncer::numTempos, 1.0);
		}
	}

	double getValueFromText(const String& text) override
	{
		if (tempoSyncMode)
			return TempoSyncer::getTempoIndex(text);
		else
			return Slider::getValueFromText(text);
	}

	
	String getTextFromValue(double value) override
	{
		if (tempoSyncMode)
			return TempoSyncer::getTempoName((int)value);
		else
			return Slider::getTextFromValue(text);
	}
#endif

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

	BrightComboBox(MainController* mc, const String& name, const String& processorId):
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

	PowerButton(MainController* mc, const String& name, const String& processorId):
		Button(name),
		ControlledObject(mc),
		connection(this, mc, processorId)
	{
		static const uint8 powerIconData[] = { 110, 109, 128, 74, 123, 67, 0, 47, 253, 67, 108, 128, 74, 123, 67, 128, 215, 0, 68, 108, 0, 216, 125, 67, 128, 215, 0, 68, 108, 0, 216, 125, 67, 0, 47, 253, 67, 108, 128, 74, 123, 67, 0, 47, 253, 67, 99, 109, 0, 1, 122, 67, 64, 232, 253, 67, 98, 147, 23, 119, 67, 252, 107, 254, 67, 0, 254, 116, 67, 123, 206, 255, 67, 0, 254,
			116, 67, 0, 183, 0, 68, 98, 0, 254, 116, 67, 119, 190, 1, 68, 75, 93, 120, 67, 160, 149, 2, 68, 128, 126, 124, 67, 160, 149, 2, 68, 98, 219, 79, 128, 67, 160, 149, 2, 68, 128, 255, 129, 67, 119, 190, 1, 68, 128, 255, 129, 67, 0, 183, 0, 68, 98, 128, 255, 129, 67, 38, 207, 255, 67, 135, 243, 128, 67, 62, 109, 254, 67, 128,
			255, 126, 67, 0, 233, 253, 67, 108, 128, 255, 126, 67, 0, 44, 255, 67, 98, 141, 76, 128, 67, 141, 156, 255, 67, 0, 216, 128, 67, 129, 57, 0, 68, 0, 216, 128, 67, 0, 183, 0, 68, 98, 0, 216, 128, 67, 224, 110, 1, 68, 86, 96, 127, 67, 96, 2, 2, 68, 128, 126, 124, 67, 96, 2, 2, 68, 98, 170, 156, 121, 67, 96, 2, 2, 68, 0, 77, 119,
			67, 224, 110, 1, 68, 0, 77, 119, 67, 0, 183, 0, 68, 98, 0, 77, 119, 67, 37, 57, 0, 68, 115, 101, 120, 67, 69, 155, 255, 67, 0, 1, 122, 67, 0, 43, 255, 67, 108, 0, 1, 122, 67, 64, 232, 253, 67, 99, 101, 0, 0 };

		p.loadPathFromData(powerIconData, sizeof(powerIconData));
		p.scaleToFit(9.0f, 9.0f, 20.0f, 20.0f, true);

		setClickingTogglesState(true);

		connection.setData<raw::Data<bool>::Bypassed<true>>();
	}

	void paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
	{
		if (isMouseOverButton)
			g.fillAll(Colour(0x10000000));

		if (isButtonDown)
			g.fillAll(Colour(0x10000000));

		g.setColour(Colour(0xFF777777));
		g.fillPath(p);

		if (getToggleState())
		{
			DropShadow s;
			s.colour = Colour(0x77FFFFFF);
			s.radius = 10;
			
			s.drawForPath(g, p);

			g.setColour(Colour(0xFF333333));
			g.fillPath(p);
		}
	}

	Path p;

	raw::UIConnection::Button connection;
};

class VCSOInterface : public Component,
					  public ControlledObject,
					  public TextButton::Listener,
					  public MainController::SampleManager::PreloadListener
{
public:

	class Tab : public Component,
		public ControlledObject
	{
	public:


		static constexpr uint32 leftColour = 0x220077FF;
		static constexpr uint32 rightColour = 0x22FF8800;

		Tab(MainController* mc, int index_) :
			ControlledObject(mc),
			index(index_)
		{};

		void addAndSetName(Component& parent, Component& c, const String& name)
		{
			parent.addAndMakeVisible(c);
			c.setName(name);
		}

		String indexed(const String& name)
		{
			return name + String(index + 1);
		}

		int index = -1;
		
	};

	class SamplerTab : public Tab
	{
	public:

		SamplerTab(MainController* mc, int index);

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

		BrightComboBox<raw::Data<String>::SampleMap> sampleMapSelector;

		StrippedSlider<SimpleEnvelope::Release> releaseSlider;
		StrippedSlider<ColourMidiProcessor::ColourValue> colourSlider;
	};

	class FXTab : public Tab
	{
	public:

		static constexpr int filterIndexes[3] = 
		{ 
			hise::FilterBank::FilterMode::StateVariableLP,
			hise::FilterBank::FilterMode::StateVariableHP,
			hise::FilterBank::FilterMode::Allpass 
		};

		FXTab(MainController* mc, int index_) :
			Tab(mc, index_),
			delayTitle(indexed("DelayTitle"), indexed("Delay "), false),
			filterTitle(indexed("FilterTitle"), indexed("Filter "), false),
			leftTimeLabel(indexed("LeftTimeLabel"), "Left Time"),
			rightTimeLabel(indexed("RightTimeLabel"), "Right Time"),
			leftFBLabel(indexed("LeftFBLabel"), "Left FB"),
			rightFBLabel(indexed("RightFBLabel"), "Right FB"),
			mixLabel(indexed("MixLabel"), "Mix"),
			frequencyLabel(indexed("FrequencyLabel"), "Frequency"),
			qLabel(indexed("QLabel"), "Q"),
			leftTimeSlider(mc, indexed("LeftTimeSlider")),
			rightTimeSlider(mc, indexed("RightTimeSlider")),
			leftFBSlider(mc, indexed("LeftFBSlider")),
			rightFBSlider(mc, indexed("RightFBSlider")),
			mixSlider(mc, indexed("MixSlider")),
			freqSlider(mc, indexed("FrequencySlider")),
			qSlider(mc, indexed("QSlider")),
			filterGraph(mc, nullptr),
			filterSelector(mc, indexed("FilterSelector"), indexed("Filter")),
			delayEnabled(mc, indexed("DelayEnabled"), indexed("Delay")),
			filterEnabled(mc, indexed("FilterEnabled"), indexed("Filter"))
		{
			setName(indexed("FXTab"));

			addAndSetName(*this, delayPanel, indexed("DelayPanel"));
			delayPanel.addAndMakeVisible(delayTitle);
			delayPanel.addAndMakeVisible(delayEnabled);
			delayPanel.addAndMakeVisible(leftTimeLabel);
			delayPanel.addAndMakeVisible(rightTimeLabel);
			delayPanel.addAndMakeVisible(mixLabel);
			
			delayPanel.addAndMakeVisible(leftTimeSlider);
			leftTimeSlider.connect(indexed("Delay"));
			leftTimeSlider.setMode(HiSlider::TempoSync);
			
			delayPanel.addAndMakeVisible(rightTimeSlider);
			rightTimeSlider.connect(indexed("Delay"));
			rightTimeSlider.setMode(HiSlider::TempoSync);

			delayPanel.addAndMakeVisible(leftFBLabel);
			delayPanel.addAndMakeVisible(rightFBLabel);

			delayPanel.addAndMakeVisible(leftFBSlider);
			leftFBSlider.connect(indexed("Delay"));
			leftFBSlider.setMode(HiSlider::NormalizedPercentage);

			delayPanel.addAndMakeVisible(rightFBSlider);
			rightFBSlider.connect(indexed("Delay"));
			rightFBSlider.setMode(HiSlider::NormalizedPercentage);

			delayPanel.addAndMakeVisible(mixSlider);
			mixSlider.setMode(HiSlider::NormalizedPercentage);
			mixSlider.connect(indexed("Delay"));

			addAndSetName(*this, filterPanel, indexed("FilterPanel"));
			
			filterPanel.addAndMakeVisible(filterTitle);
			filterPanel.addAndMakeVisible(filterEnabled);
			filterPanel.addAndMakeVisible(qLabel);
			filterPanel.addAndMakeVisible(frequencyLabel);
			filterPanel.addAndMakeVisible(freqSlider);
			freqSlider.connect(indexed("Filter"));
			freqSlider.setMode(HiSlider::Frequency);

			filterPanel.addAndMakeVisible(qSlider);
			qSlider.connect(indexed("Filter"));
			qSlider.setMode(HiSlider::Linear, 0.1, 9.0, 1.0, 0.01);

			filterPanel.addAndMakeVisible(filterGraph);


			DynamicObject::Ptr filterGraphData = new DynamicObject();
			DynamicObject::Ptr colourData = new DynamicObject();

			colourData->setProperty("bgColour", "0x000000");
			colourData->setProperty("itemColour1", "0xFF5E5E5E");
			colourData->setProperty("itemColour2", index == 0 ? (int64)leftColour : (int64)rightColour);

			filterGraphData->setProperty("Type", "FilterDisplay");
			filterGraphData->setProperty("ProcessorId", indexed("Filter"));
			filterGraphData->setProperty("ColourData", var(colourData));

			filterGraph.setOpaque(false);
			filterGraph.setName(indexed("FilterGraph"));
			filterGraph.setContent(var(filterGraphData));

			filterPanel.addAndMakeVisible(filterSelector);

			filterSelector.addItem("Low Pass", FilterBank::FilterMode::StateVariableLP);
			filterSelector.addItem("High Pass", FilterBank::FilterMode::StateVariableHP);
			filterSelector.addItem("All Pass", FilterBank::FilterMode::Allpass);
			filterSelector.setConnectionMode(raw::UIConnection::ComboBox::Id);
		};

		GreyPanel delayPanel;
		GreyPanel filterPanel;

		TitleLabel delayTitle;
		TitleLabel filterTitle;

		SliderLabel leftTimeLabel;
		SliderLabel rightTimeLabel;
		SliderLabel leftFBLabel;
		SliderLabel rightFBLabel;
		SliderLabel mixLabel;
		SliderLabel frequencyLabel;
		SliderLabel qLabel;

		StrippedSlider<DelayEffect::DelayTimeLeft> leftTimeSlider;
		StrippedSlider<DelayEffect::DelayTimeRight> rightTimeSlider;
		StrippedSlider<DelayEffect::FeedbackLeft> leftFBSlider;
		StrippedSlider<DelayEffect::FeedbackRight> rightFBSlider;
		StrippedSlider<DelayEffect::Mix> mixSlider;

		StrippedSlider<MonoFilterEffect::Frequency> freqSlider;
		StrippedSlider<MonoFilterEffect::Q> qSlider;

		PowerButton filterEnabled;
		PowerButton delayEnabled;

		hise::FloatingTile filterGraph;

		BrightComboBox<raw::Data<int>::Attribute<MonoFilterEffect::Mode>> filterSelector;
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
	juce::TextButton fxButton;
	juce::TextButton browseButton;
	juce::TextButton settingsButton;

	juce::Component editPage;

	SamplerTab tab1;
	SamplerTab tab2;
	
	juce::Component fxPage;
	FXTab fxTab1;
	FXTab fxTab2;

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