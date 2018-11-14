const var colour = Content.addKnob("Colour", 0, 0);
colour.setRange(-12, 12, 1);
function onNoteOn()
{
    local v = -1 * colour.getValue();
    
	Message.setTransposeAmount(v);
    Message.setCoarseDetune(-1 * v);
	
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
