#include "CustomComponents.h"

#if USE_RAW_FRONTEND



void GreyPanel::paint(Graphics& g)
{
	g.setColour(Colour(0x1D000000));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.0f);
}

void PageButtonLookAndFeel::drawButtonText(Graphics& g, TextButton& b, bool /*isMouseOverButton*/, bool /*isButtonDown*/)
{
	g.setFont(GLOBAL_BOLD_FONT().withHeight(16.0f));
	g.setColour(Colour(0xFF444444).withAlpha(b.getToggleState() ? 1.0f : 0.4f));
	g.drawText(b.getButtonText(), b.getLocalBounds().toFloat(), Justification::centred);
}

void PageButtonLookAndFeel::drawButtonBackground(Graphics& g, Button& b, const Colour& /*backgroundColour*/, bool isMouseOverButton, bool isButtonDown)
{
	g.setColour(Colours::black.withAlpha(0.1f));

	if (isMouseOverButton)
		g.fillRoundedRectangle(b.getLocalBounds().toFloat(), 20.0f);

	if (isButtonDown)
		g.fillRoundedRectangle(b.getLocalBounds().toFloat(), 20.0f);
}

TitleLabel::TitleLabel(const juce::String& n, const juce::String& text, bool big)
{
	setName(n);
	setText(text, dontSendNotification);
	setEditable(false, false);
	setColour(Label::backgroundColourId, Colour(0x14000000));
	setColour(Label::textColourId, Colour(0xFF595959));
	setJustificationType(Justification::centred);

	setFont(GLOBAL_BOLD_FONT().withHeight(big ? 29.0f : 18.0f));
}

SliderLabel::SliderLabel(const juce::String& n, const juce::String& text)
{
	setName(n);
	setText(text, dontSendNotification);
	setEditable(false, false);
	setColour(Label::textColourId, Colours::black);
	setJustificationType(Justification::centred);

	setFont(GLOBAL_FONT());
}

PowerButton::PowerButton(MainController* mc, const String& name, const String& processorId) :
	Button(name),
	ControlledObject(mc),
	connection(this, mc, processorId)
{
	static const uint8 powerIconData[] = { 110, 109, 128, 74, 123, 67, 0, 47, 253, 67, 108, 128, 74, 123, 67, 128, 215, 0, 68, 108, 0, 216, 125, 67, 128, 215, 0, 68, 108, 0, 216, 125, 67, 0, 47, 253, 67, 108, 128, 74, 123, 67, 0, 47, 253, 67, 99, 109, 0, 1, 122, 67, 64, 232, 253, 67, 98, 147, 23, 119, 67, 252, 107, 254, 67, 0, 254, 116, 67, 123, 206, 255, 67, 0, 254,
		116, 67, 0, 183, 0, 68, 98, 0, 254, 116, 67, 119, 190, 1, 68, 75, 93, 120, 67, 160, 149, 2, 68, 128, 126, 124, 67, 160, 149, 2, 68, 98, 219, 79, 128, 67, 160, 149, 2, 68, 128, 255, 129, 67, 119, 190, 1, 68, 128, 255, 129, 67, 0, 183, 0, 68, 98, 128, 255, 129, 67, 38, 207, 255, 67, 135, 243, 128, 67, 62, 109, 254, 67, 128,
		255, 126, 67, 0, 233, 253, 67, 108, 128, 255, 126, 67, 0, 44, 255, 67, 98, 141, 76, 128, 67, 141, 156, 255, 67, 0, 216, 128, 67, 129, 57, 0, 68, 0, 216, 128, 67, 0, 183, 0, 68, 98, 0, 216, 128, 67, 224, 110, 1, 68, 86, 96, 127, 67, 96, 2, 2, 68, 128, 126, 124, 67, 96, 2, 2, 68, 98, 170, 156, 121, 67, 96, 2, 2, 68, 0, 77, 119,
		67, 224, 110, 1, 68, 0, 77, 119, 67, 0, 183, 0, 68, 98, 0, 77, 119, 67, 37, 57, 0, 68, 115, 101, 120, 67, 69, 155, 255, 67, 0, 1, 122, 67, 0, 43, 255, 67, 108, 0, 1, 122, 67, 64, 232, 253, 67, 99, 101, 0, 0 };

	p.loadPathFromData(powerIconData, sizeof(powerIconData));
	p.scaleToFit(9.0f, 9.0f, 20.0f, 20.0f, true);

	setClickingTogglesState(true);

	connection.setData<raw::Data<bool>::Bypassed<true>>();
}

void PowerButton::paintButton(Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
	if (isMouseOverButton)
		g.fillAll(Colour(0x10000000));

	if (isButtonDown)
		g.fillAll(Colour(0x10000000));

	g.setColour(Colour(0xFF777777));
	g.fillPath(p);

	if (getToggleState())
	{
		DropShadow s;
		s.colour = Colour(0x77FFFFFF);
		s.radius = 10;

		s.drawForPath(g, p);

		g.setColour(Colour(0xFF333333));
		g.fillPath(p);
	}
}

#endif 