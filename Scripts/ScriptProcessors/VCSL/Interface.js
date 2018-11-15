Content.makeFrontInterface(1024, 768);


//! Sample Loading namespace
namespace SampleLoading
{
    const var samplers = [ Synth.getSampler("Sampler1"), Synth.getSampler("Sampler2") ];
    
    inline function sampleMapCallback(component, value)
    {
        local index = selectors.indexOf(component);
        local samplerToUse = samplers[index];
        
        Console.assertIsObjectOrArray(samplerToUse);
        
        samplerToUse.loadSampleMap(component.getItemText());
    };
    
    inline function initBoxes()
    {
        local cbList = [ Content.getComponent("InstrumentSelector1"),
                         Content.getComponent("InstrumentSelector2")];
        
        local sampleMapList = Sampler.getSampleMapList();
            
        for(cb in cbList)
        {
            cb.setControlCallback(sampleMapCallback);
            cb.set("items", "");
            
        }
            
        for(n in sampleMapList)
        {
            cbList[0].addItem(n);
            cbList[1].addItem(n);
        }
        
        return cbList;
    }
    
    const var selectors = initBoxes();
}

//! KeyColour namespace
namespace KeyColourHandling
{
    const var colourScripts = [ Synth.getMidiProcessor("ColourScript1"),
                                Synth.getMidiProcessor("ColourScript2") ];
    
    const var colourSliders = [ Content.getComponent("ColourSlider1"),
                                Content.getComponent("ColourSlider2")];
                                
    inline function colourScriptCallback(component, value)
    {
        local index = colourSliders.indexOf(component);
        colourScripts[index].setAttribute(0, value);
        dirty = true;
        //updateKeyboardColours();
    }
    
    for(cs in colourSliders)
        cs.setControlCallback(colourScriptCallback);
    
    
    const var updater = Engine.createTimerObject();
    reg dirty = false;
    
    updater.setTimerCallback(function()
    {
        if(dirty)
        {
            dirty = false;
            updateKeyboardColours();
        }
    });
    
    updater.startTimer(50);
    
    inline function isMapped(noteNumber, samplerIndex)
    {
        local noteNumberToUse = noteNumber - colourSliders[samplerIndex].getValue();
        
        return SampleLoading.samplers[samplerIndex].isNoteNumberMapped(noteNumberToUse);
    }
    
    inline function updateKeyboardColours()
    {
        local i = 0;
    
        for(i = 18; i < 127; i++)
        {
            local colour = 0x00000000;
            
            if(isMapped(i, 0))
                colour |= 0x000077FF;
            
            if(isMapped(i, 1))
                colour |= 0x00FF8800;
                
            if(colour == 0)
                colour = 0xAA555555;
            else
                colour |= 0x22000000;
                
            Engine.setKeyColour(i, colour);
        }
    }

    const var backgroundPanel = Content.getComponent("Background");

    backgroundPanel.setLoadingCallback(function(isPreloading)
    {
        if(!isPreloading)
            updateKeyboardColours();
    });
}

namespace PageHandling
{
    inline function refreshPage()
    {
        local pageToShow = currentPage == browseButton ? 1 : 0;
        
        pages[0].set("visible", currentPage == editButton);
        pages[1].set("visible", currentPage == fxButton);
        pages[2].set("visible", currentPage == browseButton);
        pages[3].set("visible", currentPage == settingsButton);
        
        editButton.repaint();
        fxButton.repaint();
        browseButton.repaint();
        settingsButton.repaint();
    }
    
    inline function makeButton(name)
    {
        local p = Content.getComponent(name + "Button");
        
        p.data.name = name;
        p.data.hover = false;
        p.data.down = false;
        
        p.setPaintRoutine(function(g)
        {
            var b = [0, 0, this.getWidth(), this.getHeight()];
            
            g.setColour(this.get("bgColour"));
            
            if(this.data.hover)
                g.fillRoundedRectangle(b, 3.0);
            
            if(this.data.down)
                g.fillRoundedRectangle(b, 3.0);
                
            g.setColour(Colours.withAlpha(0xFF444444, this == currentPage ? 1.0 : 0.2));
                
            g.setFont("Oxygen Bold", 16.0);
                
            g.drawAlignedText(this.data.name, [0, 0, this.getWidth(), this.getHeight()], "centred");
        });
        
        p.set("allowCallbacks", "Clicks & Hover");
        
        p.setMouseCallback(function(event)
        {
            this.data.hover = event.hover;
            
            if(event.clicked)
                this.data.down = true;
                
            if(event.mouseUp)
            {
                this.data.down = false;
                currentPage = this;
                refreshPage();
            }
                
            this.repaint();
        });
        
        return p;
    }
    
    const var editButton = makeButton("Edit");
    const var browseButton = makeButton("Browse");
    const var settingsButton = makeButton("Settings");
    const var fxButton = makeButton("FX");
    
    const var pages = [Content.getComponent("EditPage"),
                       Content.getComponent("FXPage"),
                       Content.getComponent("BrowsePage"), 
                       Content.getComponent("SettingsPage")];
    
    
    reg currentPage = editButton;
    refreshPage();
}

include("PowerButton.js");

PowerButton.make("FilterEnabled1");
PowerButton.make("DelayEnabled1");
PowerButton.make("FilterEnabled2");
PowerButton.make("DelayEnabled2");

const var filterModes = Engine.getFilterModeList();

const var modes = [ filterModes.StateVariableLP,
                    filterModes.StateVariableHP,
                    filterModes.Allpass ];
                    

const var Filter1 = Synth.getEffect("Filter1");
const var Filter2 = Synth.getEffect("Filter2");

inline function onFilterSelector1Control(component, value)
{
    if(value)
	    Filter1.setAttribute(Filter1.Mode, modes[value-1]);
};

Content.getComponent("FilterSelector1").setControlCallback(onFilterSelector1Control);

inline function onFilterSelector2Control(component, value)
{
    if(value)
	    Filter2.setAttribute(Filter2.Mode, modes[value-1]);
};

Content.getComponent("FilterSelector2").setControlCallback(onFilterSelector2Control);
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
