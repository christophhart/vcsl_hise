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
	};

	addSampler(0);
	addSampler(1);

	addToUserPreset<raw::Data::SampleMap>("LoadedMap1", "Sampler1");
	addToUserPreset<raw::Data::Table<0>>("VeloTable1", "Velocity1");
	addToUserPreset<raw::Data::Attribute<SimpleEnvelope::Release>>("Release1", "DefaultEnvelope1");
	addToUserPreset<raw::Data::Attribute<ColourMidiProcessor::ColourValue>>("Colour1", "Colour1");
	addToUserPreset<raw::Data::SampleMap>("LoadedMap2", "Sampler2");
	addToUserPreset<raw::Data::Table<0>>("VeloTable2", "Velocity2");
	addToUserPreset<raw::Data::Attribute<SimpleEnvelope::Release>>("Release2", "DefaultEnvelope2");
	addToUserPreset<raw::Data::Attribute<ColourMidiProcessor::ColourValue>>("Colour2", "Colour2");

	RawPluginParameter::ProcessorConnection s = { getMainController(), "DefaultEnvelope1", SimpleEnvelope::Release, {0.0f, 20000.0f, 0.1f} };
	s.range.setSkewForCentre(1000.0f);

	addParameter("Release 1", RawPluginParameter::Type::Slider, { s });

	RawPluginParameter::ProcessorConnection s2 = { getMainController(), "DefaultEnvelope2", SimpleEnvelope::Release,{ 0.0f, 20000.0f, 0.1f } };
	s2.range.setSkewForCentre(1000.0f);

	//addParameter("Release 2", RawPluginParameter::Type::Slider, { s2 });
}

VCSOInterface::SamplerTab::SamplerTab(MainController* mc, int index_) :
	ControlledObject(mc),
	index(index_),
	instrumentTitle(indexed("InstrumentTitle"), "Layer " + String(index == 0 ? "A" : "B"), false),
	veloTitle(indexed("VelocityTitle"), "Velocity " + String(index == 0 ? "A" : "B"), false),
	sampler(mc, indexed("Sampler")),
	colourLabel(indexed("ColourLabel"), "Colour"),
	releaseLabel(indexed("ReleaseLabel"), "Release"),
	velocityMod(mc, indexed("Velocity")),
	colorScript(mc, indexed("Colour"), true),
	releaseConnection(&releaseSlider, mc, indexed("DefaultEnvelope"), SimpleEnvelope::Release),
	colourConnection(&colourSlider, mc, indexed("Colour"), ColourMidiProcessor::ColourValue),
	velocityEditor(nullptr, nullptr)
{
	setName(indexed("Tab"));

	raw::Pool pool(mc, true);
	flaf.setFilmstripImage(pool.loadImage("Strip.png"), 100);

	addAndMakeVisible(instrumentPanel);
	instrumentPanel.setName(indexed("InstrumentPanel"));

	instrumentPanel.addAndMakeVisible(instrumentTitle);
	instrumentPanel.addAndMakeVisible(colourLabel);
	instrumentPanel.addAndMakeVisible(releaseLabel);

	instrumentPanel.addAndMakeVisible(sampleMapSelector);
	sampleMapSelector.setName(indexed("InstrumentSelector"));
	getMainController()->skin(sampleMapSelector);
	sampleMapSelector.addItemList(pool.getSampleMapList(), 1);
	sampleMapSelector.addListener(this);

	sampleMapSelector.setColour(HiseColourScheme::ComponentTextColourId, Colours::black);
	sampleMapSelector.setColour(HiseColourScheme::ComponentBackgroundColour, Colours::transparentBlack);
	sampleMapSelector.setColour(HiseColourScheme::ComponentOutlineColourId, Colours::transparentBlack);
	sampleMapSelector.setColour(HiseColourScheme::ComponentFillTopColourId, Colour(0x11333333));
	sampleMapSelector.setColour(HiseColourScheme::ComponentFillBottomColourId, Colour(0x20111111));

	sampler->getSampleMap()->addListener(this);
	sampleMapSelector.setText(sampler->getSampleMap()->getReference().getReferenceString(), dontSendNotification);

	instrumentPanel.addAndMakeVisible(colourSlider);
	colourSlider.setName(indexed("ColourSlider"));
	colourSlider.setRange(-12.0, 12.0, 1.0);
	colourSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	colourSlider.setLookAndFeel(&flaf);
	colourSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

	auto f = [this](float newValue)
	{
		if (auto parent = findParentComponentOfClass<VCSOInterface>())
			parent->refreshKeyboardColours();
	};

	colorScript.addParameterToWatch(0, f);

	instrumentPanel.addAndMakeVisible(releaseSlider);
	releaseSlider.setName(indexed("ReleaseSlider"));
	releaseSlider.setRange(0.0, 20000.0, 0.1);
	releaseSlider.setSkewFactorFromMidPoint(1000.0);
	releaseSlider.setTextValueSuffix(" ms");
	releaseSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	releaseSlider.setLookAndFeel(&flaf);
	releaseSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

	addAndMakeVisible(velocityPanel);
	velocityPanel.setName(indexed("VelocityPanel"));

	velocityPanel.addAndMakeVisible(veloTitle);

	velocityPanel.addAndMakeVisible(velocityEditor);

	velocityEditor.setName(indexed("Table"));
	velocityEditor.setUseFlatDesign(true);
	velocityEditor.setColour(TableEditor::ColourIds::bgColour, Colour(0x00161616));
	velocityEditor.setColour(TableEditor::ColourIds::fillColour, Colour(index == 0 ? LeftColour : rightColour));
	velocityEditor.setColour(TableEditor::ColourIds::lineColour, Colour(0xFB5F5F5F));
	velocityEditor.connectToLookupTableProcessor(velocityMod, 0);

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
	midiLabel("MIDILabel", "MIDI Sources", false)
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

	addAndMakeVisible(editButton);
	editButton.setName("EditButton");
	editButton.setButtonText("Edit");
	editButton.addListener(this);
	editButton.setRadioGroupId(1, dontSendNotification);
	editButton.setClickingTogglesState(true);
	editButton.setLookAndFeel(&plaf);

	addAndMakeVisible(browseButton);
	browseButton.setName("BrowseButton");
	browseButton.setButtonText("Browse");
	browseButton.addListener(this);
	browseButton.setRadioGroupId(1, dontSendNotification);
	browseButton.setClickingTogglesState(true);
	browseButton.setLookAndFeel(&plaf);

	addAndMakeVisible(settingsButton);
	settingsButton.setName("SettingsButton");
	settingsButton.setButtonText("Settings");
	settingsButton.addListener(this);
	settingsButton.setRadioGroupId(1, dontSendNotification);
	settingsButton.setClickingTogglesState(true);
	settingsButton.setLookAndFeel(&plaf);

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
	hise::raw::Positioner p(
	{
	 "Background", { 0, 0, 1023, 771 }, 
	 {
	  { "Keyboard", { 18, 663, 988, 108 }, {} },
	  { "HiseLogo", { 956, 15, 50, 50 }, {} },
	  { "VCSLLogo", { 15, 12, 50, 50 }, {} },
	  { "EditButton", { 265, 600, 110, 50 }, {} },
	  { "BrowseButton", { 462, 600, 110, 50 }, {} },
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
	  { "SettingsButton", { 660, 600, 110, 50 }, {} },
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
			colour |= 0x00FF0000;

		if (isMapped(i, 1))
			colour |= 0x0000FFFF;

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