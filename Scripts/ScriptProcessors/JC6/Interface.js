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
        
        local n1 = i - c1;
        local n2 = i - c2;
        
        if(sampler1.isNoteNumberMapped(n1))
            c |= 0x220077FF;
            
        if(sampler2.isNoteNumberMapped(n2))
            c |= 0x22FF8800;
        
        if(c == 0)
            c = 0x99444444;
            
        Engine.setKeyColour(i, c);
    }
}

const var Background = Content.getComponent("Background");
Background.setLoadingCallback(function(isPreloading)
{
	if(!isPreloading)
	    refreshKeys();
});


const var ColourSlider1 = Content.getComponent("ColourSlider1");
const var ColourSlider2 = Content.getComponent("ColourSlider2");

var c1 = 0;
var c2 = 0;

Background.setTimerCallback(function()
{
    var dirty = false;
    
    if(ColourSlider1.getValue() != c1 ||
       ColourSlider2.getValue() != c2)
        dirty = true;
        
    c1 = ColourSlider1.getValue();
    c2 = ColourSlider2.getValue();
    
    if(dirty)
        refreshKeys();
});

Background.startTimer(100);

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
