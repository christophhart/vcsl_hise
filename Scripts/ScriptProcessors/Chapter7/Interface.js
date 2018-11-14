Content.makeFrontInterface(1024, 768);

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

//! PageHandling

include("PageHandling.js");

//! KeyHandling

inline function refreshKeys()
{
    
    
    for(i = 0; i < 127; i++)
    {
        local c = 0x0000000;
        
        local n1 = i - c1;
        local n2 = i - c2;
        
        if(sampler1.isNoteNumberMapped(n1))
            c |= 0x00FF0000;
            
        if(sampler2.isNoteNumberMapped(n2))
            c |= 0x0000FFFF;
        
        if(c == 0)
            c = 0x99444444;
        else
            c |= 0x22000000;
            
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
