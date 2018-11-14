const var colour = Content.addKnob("colour", 0, 0);
colour.setRange(-12, 12, 1);
function onNoteOn()
{
	Message.setTransposeAmount(-colour.getValue());
	Message.setCoarseDetune(colour.getValue());
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
