#include "Raw.h"

#if USE_RAW_FRONTEND

struct UIHelpers
{
	static void drawTitle(Graphics& g, Rectangle<float> area, const String& text)
	{
		g.setColour(Colour(0x14000000));
		g.fillRect(area);
		g.setFont(GLOBAL_BOLD_FONT().withHeight(area.getHeight() - 9.0f));
		g.setColour(Colour(0xFF595959));
		g.drawText(text, area, Justification::centred);
	}
};

VCSLData::VCSLData(MainController* mc) :
	RawDataBase(mc)
{
	auto storagePtr = &storedData;

	auto addSampler = [mc, storagePtr](int index)
	{
		raw::Builder builder(mc);

		auto sampler = builder.create<ModulatorSampler>(mc->getMainSynthChain());
		sampler->setId("Sampler" + String(index+1));

		auto envelope = builder.find<SimpleEnvelope>("DefaultEnvelope").getProcessor();
		envelope->setId("DefaultEnvelope" + String(index + 1));

		auto velo = builder.create<hise::VelocityModulator>(sampler, raw::IDs::Chains::Gain);
		velo->setAttribute(VelocityModulator::UseTable, 1.0f, dontSendNotification);
		velo->setId("Velocity" + String(index + 1));

		builder.add<ColourMidiProcessor>(new ColourMidiProcessor(sampler, index), sampler, raw::IDs::Chains::Midi);

		auto fx = builder.create<hise::PolyFilterEffect>(sampler, raw::IDs::Chains::FX);
		fx->setId("Filter" + String(index + 1));

		auto delay = builder.create<hise::DelayEffect>(sampler, raw::IDs::Chains::FX);
		delay->setId("Delay" + String(index + 1));
	};

	addSampler(0);
	addSampler(1);

	addToUserPreset<raw::GenericStorage::SampleMap>("LoadedMap1", "Sampler1");
	addToUserPreset<raw::GenericStorage::Table<0>>("VeloTable1", "Velocity1");
	addToUserPreset<raw::GenericStorage::Attribute<SimpleEnvelope::Release>>("Release1", "DefaultEnvelope1");
	addToUserPreset<raw::GenericStorage::Attribute<ColourMidiProcessor::ColourValue>>("Colour1", "Colour1");
	addToUserPreset<raw::GenericStorage::SampleMap>("LoadedMap2", "Sampler2");
	addToUserPreset<raw::GenericStorage::Table<0>>("VeloTable2", "Velocity2");
	addToUserPreset<raw::GenericStorage::Attribute<SimpleEnvelope::Release>>("Release2", "DefaultEnvelope2");
	addToUserPreset<raw::GenericStorage::Attribute<ColourMidiProcessor::ColourValue>>("Colour2", "Colour2");

	addToUserPreset<raw::GenericStorage::Bypassed<true>>("Delay1Enabled", "Delay1");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::DelayTimeLeft>>("DelayLeft1", "Delay1");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::DelayTimeRight>>("DelayRight1", "Delay1");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::FeedbackLeft>>("FeedbackLeft1", "Delay1");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::FeedbackRight>>("FeedbackRight1", "Delay1");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::Mix>>("Mix1", "Delay1");

	addToUserPreset<raw::GenericStorage::Bypassed<true>>("Delay2Enabled", "Delay2");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::DelayTimeLeft>>("DelayLeft2", "Delay2");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::DelayTimeRight>>("DelayRight2", "Delay2");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::FeedbackLeft>>("FeedbackLeft2", "Delay2");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::FeedbackRight>>("FeedbackRight2", "Delay2");
	addToUserPreset<raw::GenericStorage::Attribute<DelayEffect::Mix>>("Mix2", "Delay2");

#define raw_parameter(x, name) raw::PluginParameter<raw::Data<float>::Attribute<x>>(getMainController(), name); 

	auto p1 = new raw_parameter(SimpleEnvelope::Release, "Release 1");
	p1->setup(raw::IDs::UIWidgets::Slider, "DefaultEnvelope1", { 0.0f, 20000.0f, 0.1f }, 1000.0f);
	addParameter(p1);

	auto p2 = new raw_parameter(SimpleEnvelope::Release, "Release 2");
	p2->setup(raw::IDs::UIWidgets::Slider, "DefaultEnvelope2", { 0.0f, 20000.0f, 0.1f }, 1000.0f);
	addParameter(p2);
	
	auto p3 = new raw_parameter(ColourMidiProcessor::ColourValue, "Colour 1");
	p3->setup(raw::IDs::UIWidgets::Slider, "Colour1", { -12.0, 12.0, 1.0 }, 0.0f);
	addParameter(p3);

	auto p4 = new raw_parameter(ColourMidiProcessor::ColourValue, "Colour 2");
	p4->setup(raw::IDs::UIWidgets::Slider, "Colour2", { -12.0, 12.0, 1.0 }, 0.0f);
	addParameter(p4);
	
}

VCSOInterface::SamplerTab::SamplerTab(MainController* mc, int index_) :
	Tab(mc, index_),
	instrumentTitle(indexed("InstrumentTitle"), "Layer " + String(index == 0 ? "A" : "B"), false),
	veloTitle(indexed("VelocityTitle"), "Velocity " + String(index == 0 ? "A" : "B"), false),
	sampler(mc, indexed("Sampler")),
	colourLabel(indexed("ColourLabel"), "Colour"),
	releaseLabel(indexed("ReleaseLabel"), "Release"),
	velocityMod(mc, indexed("Velocity")),
	colorScript(mc, indexed("Colour"), true),
	releaseSlider(mc, indexed("ReleaseSlider")),
	colourSlider(mc, indexed("ColourSlider")),
	sampleMapSelector(mc, indexed("InstrumentSelector"), indexed("Sampler")),
	velocityEditor(nullptr, nullptr)
{
	setName(indexed("Tab"));

	addAndMakeVisible(instrumentPanel);
	instrumentPanel.setName(indexed("InstrumentPanel"));
	instrumentPanel.addAndMakeVisible(instrumentTitle);
	instrumentPanel.addAndMakeVisible(colourLabel);
	instrumentPanel.addAndMakeVisible(releaseLabel);
	instrumentPanel.addAndMakeVisible(sampleMapSelector);
	sampleMapSelector.addItemList(raw::Pool(mc, true).getSampleMapList(), 1);
	sampleMapSelector.setConnectionMode(raw::UIConnection::ComboBox::Text);
	sampleMapSelector.setTextWhenNothingSelected("Select Sampleset");

	instrumentPanel.addAndMakeVisible(colourSlider);	
	colourSlider.connect(indexed("Colour"));
	colourSlider.setMode(HiSlider::Linear, -12.0, 12.0, 0.0, 1.0);

	auto f = [this](float newValue)
	{
		if (auto parent = findParentComponentOfClass<VCSOInterface>())
			parent->refreshKeyboardColours();
	};

	colorScript.addParameterToWatch(0, f);

	instrumentPanel.addAndMakeVisible(releaseSlider);
	releaseSlider.connect(indexed("DefaultEnvelope"));
	releaseSlider.setMode(HiSlider::Frequency);

	addAndMakeVisible(velocityPanel);
	velocityPanel.setName(indexed("VelocityPanel"));

	velocityPanel.addAndMakeVisible(veloTitle);

	velocityPanel.addAndMakeVisible(velocityEditor);

	velocityEditor.setName(indexed("Table"));
	velocityEditor.setUseFlatDesign(true);
	velocityEditor.setColour(TableEditor::ColourIds::bgColour, Colour(0x00161616));
	velocityEditor.setColour(TableEditor::ColourIds::fillColour, Colour(index == 0 ? leftColour : rightColour));
	velocityEditor.setColour(TableEditor::ColourIds::lineColour, Colour(0xFB5F5F5F));
	velocityEditor.connectToLookupTableProcessor(velocityMod, 0);
	velocityEditor.setLineThickness(1.0f);
}

VCSOInterface::VCSOInterface(VCSLData* data) :
	ControlledObject(data->getMainController()),
	MainController::SampleManager::PreloadListener(data->getMainController()->getSampleManager()),
	mainTitle("Title", "VCSL - HISE Edition", true),
	keyboard(getMainController(), nullptr),
	presetBrowser(data->getMainController()),
	tab1(getMainController(), 0),
	tab2(getMainController(), 1),
	audioSettings(getMainController(), nullptr),
	midiSources(getMainController(), nullptr),
	audioLabel("AudioSettingsLabel", "Audio Settings", false),
	midiLabel("MIDILabel", "MIDI Sources", false),
	fxTab1(getMainController(), 0),
	fxTab2(getMainController(), 1)
{
	auto& glaf = data->getMainController()->getGlobalLookAndFeel();
	glaf.setColour(PopupMenu::ColourIds::backgroundColourId, Colour(0xFFAAAAAA));
	glaf.setColour(PopupMenu::ColourIds::headerTextColourId, Colour(0xFF333333));
	glaf.setColour(PopupMenu::ColourIds::textColourId, Colour(0xFF333333));
	glaf.setColour(PopupMenu::ColourIds::highlightedBackgroundColourId, Colour(0xFF333333));
	glaf.setColour(PopupMenu::ColourIds::highlightedTextColourId, Colour(0xFF999999));

	setName("Background");

	addAndMakeVisible(mainTitle);
	addAndMakeVisible(keyboard);

	keyboard.setName("Keyboard");
	keyboard.setContent({
		{ "Type", "Keyboard" },
		{ "KeyWidth", 19 },
		{ "LowKey", 21 },
		{ "DefaultAppearance", false },
		{ "UseVectorGraphics", true }
	});

	addAndMakeVisible(editPage);
	editPage.setName("EditPage");

	editPage.addAndMakeVisible(tab1);
	editPage.addAndMakeVisible(tab2);
	
	addAndMakeVisible(fxPage);
	fxPage.setName("FXPage");
	fxPage.addAndMakeVisible(fxTab1);
	fxPage.addAndMakeVisible(fxTab2);

	addAndMakeVisible(browsePage);
	browsePage.setName("BrowsePage");
	browsePage.addAndMakeVisible(presetBrowserBg);
	presetBrowserBg.setName("PresetBrowserBg");
	presetBrowserBg.addAndMakeVisible(presetBrowser);

	presetBrowser.setName("PresetBrowser");
	presetBrowser.pblaf.modalBackgroundColour = Colours::black.withAlpha(0.3f);

	PresetBrowser::Options newOptions;

	newOptions.backgroundColour = Colours::transparentBlack;
	newOptions.textColour = Colour(0xFF545454);
	newOptions.highlightColour = Colour(0xFF545454);
	newOptions.showNotesLabel = false;
	newOptions.showFavoriteIcons = false;
	newOptions.numColumns = 1,

	presetBrowser.setOptions(newOptions);

	addAndSetName(*this, settingsPage, "SettingsPage");
	addAndSetName(settingsPage, settingsBg, "SettingsBg");
	settingsBg.addAndMakeVisible(audioLabel);
	settingsBg.addAndMakeVisible(midiLabel);

	addAndSetName(settingsBg, audioSettings, "Settings");

	DynamicObject::Ptr cData = new DynamicObject();
	cData->setProperty("textColour", "0xFF444444");

	audioSettings.setContent({
		{ "Type", "CustomSettings" },
		{ "ColourData", var(cData)},
		{ "Driver", true },
		{ "Device", true },
		{ "Output", true },
		{ "BufferSize", true },
		{ "SampleRate", true },
		{ "GlobalBPM", false },
		{ "StreamingMode", false },
		{ "GraphicRendering", false },
		{ "ScaleFactor", true },
		{ "SustainCC", false },
		{ "VoiceAmountMultiplier", false },
		{ "ClearMidiCC", false },
		{ "SampleLocation", true },
		{ "DebugMode", true }
	});

	addAndSetName(settingsBg, midiSources, "MidiSources");

	midiSources.setContent({ 
		{ "Type", "MidiSources" }, 
		{ "ColourData", var(cData) } 
	});

	midiSources.setOpaque(false);

	audioSettings.setOpaque(false);
	audioSettings.setColour(CustomSettingsWindow::ColourIds::textColour, Colour(0xFF444444));	

	addAndMakeVisible(hiseLogo);
	hiseLogo.setName("HiseLogo");
	hiseLogo.setImage(raw::Pool(getMainController(), true).loadImage("logo_new.png"));

	addAndMakeVisible(vcslLogo);
	vcslLogo.setName("VCSLLogo");
	vcslLogo.setImage(raw::Pool(getMainController(), true).loadImage("VCSL_Color_600x600.png"));

	auto makeButton = [this](Button& b, const String& s)
	{
		addAndMakeVisible(b);
		b.setName(s + "Button");
		b.setButtonText(s);
		b.addListener(this);
		b.setRadioGroupId(1, dontSendNotification);
		b.setClickingTogglesState(true);
		b.setLookAndFeel(&plaf);
	};

	makeButton(editButton, "Edit");
	makeButton(fxButton, "FX");
	makeButton(browseButton, "Browse");
	makeButton(settingsButton, "Settings");

	showPage(&settingsButton);

	refreshKeyboardColours();

	setSize(1024, 768);
}

void VCSOInterface::paint(Graphics& g)
{
	g.setGradientFill(ColourGradient(Colour(0xFFDDDDDD), 0.0f, 0.0f,
									 Colour(0xFFBDBDBD), 0.0f, (float)getHeight(), false));
	g.fillAll();
}

void VCSOInterface::resized()
{
	raw::Positioner p(
	{
	 "Background", { 0, 1, 1024, 768 }, 
	 {
	  { "Keyboard", { 18, 663, 988, 108 }, {} },
	  { "HiseLogo", { 956, 15, 50, 50 }, {} },
	  { "VCSLLogo", { 15, 12, 50, 50 }, {} },
	  { "EditButton", { 197, 600, 110, 50 }, {} },
	  { "BrowseButton", { 540, 600, 110, 50 }, {} },
	  { "Title", { 85, 20, 853, 39 }, {} },
	  {
	   "EditPage", { 0, 0, 1034, 588 }, 
	   {
		{
		 "Tab1", { 0, 0, 515, 585 }, 
		 {
		  {
		   "InstrumentPanel1", { 85, 89, 414, 158 }, 
		   {
			{ "InstrumentTitle1", { 17, 17, 381, 38 }, {} },
			{ "InstrumentSelector1", { 111, 83, 181, 32 }, {} },
			{ "ColourLabel1", { -12, 122, 128, 28 }, {} },
			{ "ReleaseLabel1", { 306, 121, 128, 28 }, {} },
			{ "ReleaseSlider1", { 334, 66, 70, 60 }, {} },
			{ "ColourSlider1", { 16, 67, 70, 60 }, {} }
		   }
		  },
		  {
		   "VelocityPanel1", { 85, 269, 414, 308 }, 
		   {
			{ "Table1", { 18, 69, 377, 216 }, {} },
			{ "VelocityTitle1", { 17, 17, 379, 38 }, {} }
		   }
		  }
		 }
		},
		{
		 "Tab2", { 508, 0, 515, 585 }, 
		 {
		  {
		   "InstrumentPanel2", { 17, 89, 414, 158 }, 
		   {
			{ "InstrumentTitle2", { 17, 17, 381, 38 }, {} },
			{ "InstrumentSelector2", { 111, 83, 181, 32 }, {} },
			{ "ReleaseLabel2", { 306, 121, 128, 28 }, {} },
			{ "ReleaseSlider2", { 334, 66, 70, 60 }, {} },
			{ "ColourLabel2", { -12, 122, 128, 28 }, {} },
			{ "ColourSlider2", { 16, 67, 70, 60 }, {} }
		   }
		  },
		  {
		   "VelocityPanel2", { 17, 269, 414, 308 }, 
		   {
			{ "Table2", { 18, 69, 377, 216 }, {} },
			{ "VelocityTitle2", { 17, 17, 380, 38 }, {} }
		   }
		  }
		 }
		}
	   }
	  },
	  {
	   "BrowsePage", { -1, -1, 1034, 588 }, 
	   {
		{
		 "PresetBrowserBg", { 85, 97, 855, 478 }, 
		 {
		  { "PresetBrowser", { 17, 18, 823, 443 }, {} }
		 }
		}
	   }
	  },
	  { "SettingsButton", { 712, 600, 110, 50 }, {} },
	  {
	   "SettingsPage", { 0, 1, 1034, 588 }, 
	   {
		{
		 "SettingsBg", { 85, 97, 855, 478 }, 
		 {
		  { "AudioSettingsLabel", { 17, 22, 400, 38 }, {} },
		  { "Settings", { 45, 62, 374, 388 }, {} },
		  { "MidiSources", { 465, 73, 330, 321 }, {} },
		  { "MIDILabel", { 434, 22, 400, 38 }, {} }
		 }
		}
	   }
	  },
	  { "FXButton", { 368, 600, 110, 50 }, {} },
	  {
	   "FXPage", { 0, 0, 1034, 588 }, 
	   {
		{
		 "FXTab1", { 0, 0, 515, 585 }, 
		 {
		  {
		   "DelayPanel1", { 85, 416, 414, 158 }, 
		   {
			{ "DelayTitle1", { 17, 17, 381, 38 }, {} },
			{ "LeftTimeLabel1", { -12, 122, 128, 28 }, {} },
			{ "MixLabel1", { 318, 122, 108, 28 }, {} },
			{ "MixSlider1", { 337, 67, 70, 60 }, {} },
			{ "LeftTimeSlider1", { 19, 67, 70, 60 }, {} },
			{ "RightTimeSlider1", { 98, 67, 70, 60 }, {} },
			{ "RightTimeLabel1", { 70, 122, 128, 28 }, {} },
			{ "DelayEnabled1", { 17, 17, 381, 38 }, {} },
			{ "LeftFBSlider1", { 178, 67, 70, 60 }, {} },
			{ "LeftFBLabel1", { 146, 122, 128, 28 }, {} },
			{ "RightFBSlider1", { 257, 67, 70, 60 }, {} },
			{ "RightFBLabel1", { 227, 122, 128, 28 }, {} }
		   }
		  },
		  {
		   "FilterPanel1", { 85, 89, 414, 308 }, 
		   {
			{ "FrequencyLabel1", { 8, 272, 128, 28 }, {} },
			{ "FilterGraph1", { 16, 66, 380, 140 }, {} },
			{ "FrequencySlider1", { 36, 217, 70, 60 }, {} },
			{ "FilterTitle1", { 17, 16, 379, 38 }, {} },
			{ "QLabel1", { 145, 272, 128, 28 }, {} },
			{ "QSlider1", { 173, 217, 70, 60 }, {} },
			{ "FilterSelector1", { 275, 234, 121, 32 }, {} },
			{ "FilterEnabled1", { 17, 16, 379, 38 }, {} }
		   }
		  }
		 }
		},
		{
		 "FXTab2", { 500, 0, 515, 585 }, 
		 {
		  {
		   "DelayPanel2", { 25, 416, 414, 158 }, 
		   {
			{ "DelayTitle2", { 17, 17, 381, 38 }, {} },
			{ "LeftTimeLabel2", { -12, 122, 128, 28 }, {} },
			{ "MixLabel2", { 318, 122, 108, 28 }, {} },
			{ "MixSlider2", { 337, 67, 70, 60 }, {} },
			{ "LeftTimeSlider2", { 19, 67, 70, 60 }, {} },
			{ "RightTimeSlider2", { 98, 67, 70, 60 }, {} },
			{ "RightTimeLabel2", { 70, 122, 128, 28 }, {} },
			{ "DelayEnabled2", { 17, 17, 381, 38 }, {} },
			{ "LeftFBSlider2", { 178, 67, 70, 60 }, {} },
			{ "LeftFBLabel2", { 146, 122, 128, 28 }, {} },
			{ "RightFBSlider2", { 257, 67, 70, 60 }, {} },
			{ "RightFBLabel2", { 227, 122, 128, 28 }, {} }
		   }
		  },
		  {
		   "FilterPanel2", { 25, 89, 414, 308 }, 
		   {
			{ "FrequencyLabel2", { 8, 272, 128, 28 }, {} },
			{ "FilterGraph2", { 16, 66, 380, 140 }, {} },
			{ "FrequencySlider2", { 36, 217, 70, 60 }, {} },
			{ "FilterTitle2", { 17, 16, 379, 38 }, {} },
			{ "QLabel2", { 145, 272, 128, 28 }, {} },
			{ "QSlider2", { 173, 217, 70, 60 }, {} },
			{ "FilterSelector2", { 275, 234, 121, 32 }, {} },
			{ "FilterEnabled2", { 17, 16, 379, 38 }, {} }
		   }
		  }
		 }
		}
	   }
	  }
	 }
	});

	p.apply(*this);
	p.printSummary();
}


bool VCSOInterface::isMapped(int noteNumber, int samplerIndex) const
{
	auto& tabToUse = samplerIndex == 1 ? tab2 : tab1;
	int noteNumberToUse = noteNumber - (int)tabToUse.colourSlider.getValue();
	return tabToUse.sampler->isNoteNumberMapped(noteNumberToUse);
}

void VCSOInterface::refreshKeyboardColours()
{
	for (int i = 18; i < 127; i++)
	{
		uint32 colour = 0x00000000;

		if (isMapped(i, 0))
			colour |= Tab::leftColour;

		if (isMapped(i, 1))
			colour |= Tab::rightColour;

		if (colour == 0)
			colour = 0x77555555;
		else
			colour |= 0x22000000;

		getMainController()->setKeyboardCoulour(i, Colour(colour));
	}
}

void VCSOInterface::buttonClicked(Button* b)
{
	showPage(b);
}

void VCSOInterface::showPage(Button* b)
{
	editPage.setVisible(b == &editButton);
	browsePage.setVisible(b == &browseButton);
	settingsPage.setVisible(b == &settingsButton);
	fxPage.setVisible(b == &fxButton);
}

/** This method needs to be implemented and your initial preset structure must be defined here. */
FrontendProcessor::RawDataBase* FrontendProcessor::createPresetRaw()
{
	return new VCSLData(this);
}

Component* VCSLData::createEditor()
{
	return new VCSOInterface(this);
}

#endif