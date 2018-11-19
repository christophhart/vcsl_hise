#pragma once

#include "JuceHeader.h"

#if USE_RAW_FRONTEND


/** A hardcoded version of the colour script. */
class ColourMidiProcessor : public hise::HardcodedScriptProcessor
{
public:

	/** Set the name and default ID of the processor. */
	SET_PROCESSOR_NAME("Colour", "Colour");

	enum Parameters
	{
		ColourValue = 0, ///< You can access the colour value by its index (order of definition)
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

	/** The ownership is handled by the Content object and guaranteed to exceed this member variable. */
	hise::ScriptingApi::Content::ScriptSlider* colour;

	JUCE_DECLARE_WEAK_REFERENCEABLE(ColourMidiProcessor);
};

class VCSLData : public FrontendProcessor::RawDataBase
{
public:

	VCSLData(MainController* mc);

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

#endif 