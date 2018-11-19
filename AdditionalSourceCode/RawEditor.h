#pragma once

#include "Raw.h"
#include "CustomComponents.h"

#if USE_RAW_FRONTEND

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