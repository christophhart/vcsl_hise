Content.makeFrontInterface(1024, 768);

//! PageHandling

include("PageHandling.js");

include("PowerButton.js");

PowerButton.make("DelayEnabled1");
PowerButton.make("DelayEnabled2");
PowerButton.make("FilterEnabled1");
PowerButton.make("FilterEnabled2");

//! Sampler 1

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

//! Sampler 2

const var sampler2 = Synth.getSampler("Sampler2");
const var InstrumentSelector2 = Content.getComponent("InstrumentSelector2");

InstrumentSelector2.set("items", sampleMaps.join("\n"));

inline function onInstrumentSelector2Control(component, value)
{
    if(value)
	    sampler2.loadSampleMap(component.getItemText());
};

InstrumentSelector2.setControlCallback(onInstrumentSelector2Control);

//! Filter modes

const var list = Engine.getFilterModeList();
const var filterIds = [list.StateVariableLP, list.StateVariableHP, list.Allpass];

const var Filter1 = Synth.getEffect("Filter1");
const var Filter2 = Synth.getEffect("Filter2");

inline function onFilterSelector1Control(component, value)
{
	if(value)
    {
        local v = filterIds[value-1];
        Filter1.setAttribute(Filter1.Mode, v);
    }
};

Content.getComponent("FilterSelector1").setControlCallback(onFilterSelector1Control);

inline function onFilterSelector2Control(component, value)
{
	if(value)
    {
        local v = filterIds[value-1];
        Filter2.setAttribute(Filter2.Mode, v);
    }
};

Content.getComponent("FilterSelector2").setControlCallback(onFilterSelector2Control);



//! KeyHandling

inline function refreshKeys()
{
    for(i = 0; i < 127; i++)
    {
        local c = 0x0000000;
        
        if(sampler1.isNoteNumberMapped(i))
            c |= 0x220077FF;
            
        if(sampler2.isNoteNumberMapped(i))
            c |= 0x22FF8800;
        
        if(c == 0)
            c = 0x99444444;
            
        Engine.setKeyColour(i, c);
    }
}

refreshKeys();
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
