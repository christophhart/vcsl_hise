
namespace PageHandling
{
    inline function refreshPage()
    {
        local pageToShow = currentPage == browseButton ? 1 : 0;
        
        pages[0].set("visible", currentPage == editButton);
        pages[1].set("visible", currentPage == browseButton);
        pages[2].set("visible", currentPage == settingsButton);
        
        editButton.repaint();
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
                
            g.setColour(Colours.withAlpha(this.get("textColour"), this == currentPage ? 1.0 : 0.2));
                
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
    
    const var pages = [Content.getComponent("EditPage"), Content.getComponent("BrowsePage"), Content.getComponent("SettingsPage")];
    
    reg currentPage = editButton;
    refreshPage();
}

