Content.makeFrontInterface(1024, 768);

const var sampler1 = Synth.getSampler("Sampler1");
const var sampleMaps = Sampler.getSampleMapList();
const var InstrumentSelector1 = Content.getComponent("InstrumentSelector1");

InstrumentSelector1.set("items", sampleMaps.join("\n"));

inline function onInstrumentSelector1Control(component, value)
{
    if(value)
	    sampler1.loadSampleMap(component.getItemText());
};

InstrumentSelector1.setControlCallback(onInstrumentSelector1Control);

const var sampler2 = Synth.getSampler("Sampler2");
const var InstrumentSelector2 = Content.getComponent("InstrumentSelector2");

InstrumentSelector2.set("items", sampleMaps.join("\n"));

inline function onInstrumentSelector2Control(component, value)
{
    if(value)
	    sampler2.loadSampleMap(component.getItemText());
};

InstrumentSelector2.setControlCallback(onInstrumentSelector2Control);


include("PageHandling.js");
function onNoteOn()
{
	
}
function onNoteOff()
{
	
}
function onController()
{
	
}
function onTimer()
{
	
}
function onControl(number, value)
{
	
}
