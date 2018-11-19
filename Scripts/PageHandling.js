
namespace PageHandling
{
    /** Factory function for the page swap buttons. */
    inline function makeButton(name)
    {
        // Grab a reference for each panel
        local p = Content.getComponent(name + "Button");
        
        // Set a few properties
        p.data.name = name;
        p.data.hover = false;
        p.data.down = false;
        
        /** Define the paint routine. */
        p.setPaintRoutine(function(g)
        {
            // create a rectangle for the bounds
            var b = [0, 0, this.getWidth(), this.getHeight()];
            
            g.setColour(0x22111111);
            
            if(this.data.hover)
                g.fillRoundedRectangle(b, 3.0);
            
            if(this.data.down)
                g.fillRoundedRectangle(b, 3.0);
                
            g.setColour(Colours.withAlpha(0xFF555555, this == currentPage ? 1.0 : 0.2));
            g.setFont("Oxygen Bold", 16.0);
            g.drawAlignedText(this.data.name, b, "centred");
        });
        
        /** Make sure it receives the mouse callbacks. */
        p.set("allowCallbacks", "Clicks & Hover");
        
        
        /** Define the mouse behaviour. */
        p.setMouseCallback(function(event)
        {
            this.data.hover = event.hover;
            
            if(event.clicked)
                this.data.down = true;
                
            if(event.mouseUp)
            {
                this.data.down = false;
                
                // Store the reference to the "global" variable and refresh the page state
                currentPage = this;
                refreshPage();
            }
            
            this.repaint();
        });
        
        return p;
    }
    
    /** displays the page that was selected. */
    inline function refreshPage()
    {
        pages[0].set("visible", currentPage == editButton);
        pages[1].set("visible", currentPage == fxButton);
        pages[2].set("visible", currentPage == browseButton);
        pages[3].set("visible", currentPage == settingsButton);
        
        // Make sure the buttons are updated
        editButton.repaint();
        fxButton.repaint();
        browseButton.repaint();
        settingsButton.repaint();
    }
    
    // Create references to the button panels
    const var editButton = makeButton("Edit");
    const var fxButton = makeButton("FX");
    const var browseButton = makeButton("Browse");
    const var settingsButton = makeButton("Settings");
    
    // Create references to the pages in an array
    const var pages = [Content.getComponent("EditPage"), 
                       Content.getComponent("FXPage"), 
                       Content.getComponent("BrowsePage"), 
                       Content.getComponent("SettingsPage")];
    
    // temporary variable that stores the last clicked button
    reg currentPage = editButton;
    
    // Update the page visibility on initialisation
    refreshPage();
}

