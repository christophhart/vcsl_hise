#include "Raw.h"

#if USE_RAW_FRONTEND

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


/** This method needs to be implemented and your initial preset structure must be defined here. */
FrontendProcessor::RawDataBase* FrontendProcessor::createPresetRaw()
{
	return new VCSLData(this);
}


#endif