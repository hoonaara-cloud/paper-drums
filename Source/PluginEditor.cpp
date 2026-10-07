#include "PluginEditor.h"

namespace
{
    const auto paperInk = juce::Colour(0xff3a3027);
    const auto paperRed = juce::Colour(0xffc35e4d);
    const auto paperBlue = juce::Colour(0xff4d8292);
    const auto paperGreen = juce::Colour(0xff6c8b57);
    const auto paperGold = juce::Colour(0xffc99548);
    const auto paperPurple = juce::Colour(0xff8a6b88);
}

PaperDrumsAudioProcessorEditor::PaperDrumsAudioProcessorEditor(PaperDrumsAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setResizable(true, true);
    setSize(760, 570);
    setResizeLimits(560, 430, 1300, 1000);
    startTimerHz(30);
}

void PaperDrumsAudioProcessorEditor::resized()
{
    const auto paper = getLocalBounds().toFloat().reduced(32.0f, 30.0f);
    const auto w = paper.getWidth();
    const auto h = paper.getHeight();

    padAreas[PaperDrumsAudioProcessor::Kick] = { paper.getX() + w * 0.06f,
                                                  paper.getY() + h * 0.52f,
                                                  w * 0.36f, h * 0.31f };
    padAreas[PaperDrumsAudioProcessor::Snare] = { paper.getX() + w * 0.53f,
                                                   paper.getY() + h * 0.55f,
                                                   w * 0.30f, h * 0.26f };
    padAreas[PaperDrumsAudioProcessor::Hat] = { paper.getX() + w * 0.64f,
                                                 paper.getY() + h * 0.13f,
                                                 w * 0.25f, h * 0.22f };
    padAreas[PaperDrumsAudioProcessor::Tear] = { paper.getX() + w * 0.07f,
                                                  paper.getY() + h * 0.14f,
                                                  w * 0.29f, h * 0.24f };
    padAreas[PaperDrumsAudioProcessor::Crumple] = { paper.getX() + w * 0.36f,
                                                     paper.getY() + h * 0.30f,
                                                     w * 0.25f, h * 0.24f };
}

void PaperDrumsAudioProcessorEditor::drawLabel(juce::Graphics& g,
                                               const juce::String& text,
                                               juce::Point<float> centre,
                                               float size,
                                               juce::Colour colour,
                                               juce::Justification justification)
{
    g.setColour(colour);
    g.setFont(juce::Font(juce::FontOptions(size).withStyle("Bold")));
    g.drawText(text, juce::Rectangle<float>(centre.x - 100.0f, centre.y - size * 0.65f,
                                             200.0f, size * 1.3f), justification, false);
}

void PaperDrumsAudioProcessorEditor::drawIndicator(juce::Graphics& g,
                                                    juce::Point<float> point,
                                                    float glow,
                                                    juce::Colour colour)
{
    if (glow > 0.01f)
    {
        g.setColour(colour.withAlpha(0.20f * glow));
        g.fillEllipse(point.x - 17.0f, point.y - 17.0f, 34.0f, 34.0f);
    }

    g.setColour(colour.withAlpha(0.22f + 0.78f * glow));
    g.fillEllipse(point.x - 5.0f, point.y - 5.0f, 10.0f, 10.0f);
}

void PaperDrumsAudioProcessorEditor::drawKick(juce::Graphics& g,
                                              juce::Rectangle<float> area,
                                              float glow)
{
    const auto centre = area.getCentre();
    const auto radius = juce::jmin(area.getWidth(), area.getHeight()) * 0.37f;
    const auto circle = juce::Rectangle<float>(centre.x - radius, centre.y - radius,
                                                radius * 2.0f, radius * 2.0f);

    g.setColour(paperRed.withAlpha(0.12f + glow * 0.26f));
    g.fillEllipse(circle);
    g.setColour(paperInk.withAlpha(0.83f));
    g.drawEllipse(circle, 2.5f);
    g.drawEllipse(circle.reduced(10.0f), 1.0f);

    g.setColour(paperRed.withAlpha(0.55f + glow * 0.45f));
    g.drawLine(centre.x - radius * 0.55f, centre.y + radius * 0.40f,
               centre.x + radius * 0.55f, centre.y + radius * 0.40f, 2.0f);
    drawLabel(g, "KICK", { centre.x, centre.y - 2.0f }, 18.0f, paperInk);
    drawLabel(g, "paper body", { centre.x, centre.y + 27.0f }, 10.0f, paperInk.withAlpha(0.65f));
    drawIndicator(g, { centre.x + radius + 9.0f, centre.y - radius + 8.0f }, glow, paperRed);
}

void PaperDrumsAudioProcessorEditor::drawSnare(juce::Graphics& g,
                                               juce::Rectangle<float> area,
                                               float glow)
{
    const auto drum = area.reduced(area.getWidth() * 0.14f, area.getHeight() * 0.22f);
    g.setColour(paperBlue.withAlpha(0.12f + glow * 0.28f));
    g.fillEllipse(drum);
    g.setColour(paperInk.withAlpha(0.84f));
    g.drawEllipse(drum, 2.2f);
    g.drawLine(drum.getX() + 12.0f, drum.getCentreY(), drum.getRight() - 12.0f, drum.getCentreY(), 1.5f);
    g.drawLine(drum.getCentreX(), drum.getY() + 5.0f, drum.getCentreX(), drum.getBottom() - 5.0f, 1.0f);
    drawLabel(g, "SNARE", drum.getCentre() - juce::Point<float>(0.0f, 3.0f), 16.0f, paperInk);
    drawLabel(g, "tap / slap", drum.getCentre() + juce::Point<float>(0.0f, 24.0f), 10.0f, paperInk.withAlpha(0.65f));
    drawIndicator(g, { drum.getRight() + 8.0f, drum.getCentreY() - 5.0f }, glow, paperBlue);
}

void PaperDrumsAudioProcessorEditor::drawHat(juce::Graphics& g,
                                             juce::Rectangle<float> area,
                                             float glow)
{
    const auto centre = area.getCentre();
    const auto width = area.getWidth() * 0.64f;
    const auto top = juce::Rectangle<float>(centre.x - width * 0.5f, centre.y - 20.0f, width, 18.0f);
    const auto bottom = top.translated(0.0f, 24.0f);

    g.setColour(paperGold.withAlpha(0.13f + glow * 0.32f));
    g.fillEllipse(top.expanded(4.0f, 5.0f));
    g.fillEllipse(bottom.expanded(4.0f, 5.0f));
    g.setColour(paperInk.withAlpha(0.83f));
    g.drawEllipse(top, 2.0f);
    g.drawEllipse(bottom, 2.0f);
    g.drawLine(centre.x, top.getBottom(), centre.x, bottom.getY(), 2.0f);
    g.drawLine(centre.x - 25.0f, centre.y + 28.0f, centre.x + 25.0f, centre.y + 28.0f, 1.0f);
    drawLabel(g, "HAT", { centre.x, centre.y + 54.0f }, 15.0f, paperInk);
    drawLabel(g, "pencil ticks", { centre.x, centre.y + 73.0f }, 10.0f, paperInk.withAlpha(0.65f));
    drawIndicator(g, { top.getRight() + 9.0f, top.getCentreY() }, glow, paperGold);
}

void PaperDrumsAudioProcessorEditor::drawTear(juce::Graphics& g,
                                              juce::Rectangle<float> area,
                                              float glow)
{
    const auto x = area.getX() + area.getWidth() * 0.12f;
    const auto y = area.getCentreY();
    juce::Path tear;
    tear.startNewSubPath(x, y - 21.0f);
    tear.lineTo(x + 20.0f, y - 5.0f);
    tear.lineTo(x + 35.0f, y - 19.0f);
    tear.lineTo(x + 54.0f, y + 4.0f);
    tear.lineTo(x + 73.0f, y - 14.0f);
    tear.lineTo(x + 94.0f, y + 8.0f);
    tear.lineTo(x + 113.0f, y - 3.0f);
    tear.lineTo(x + 132.0f, y + 19.0f);
    tear.lineTo(x + 110.0f, y + 14.0f);
    tear.lineTo(x + 93.0f, y + 29.0f);
    tear.lineTo(x + 73.0f, y + 16.0f);
    tear.lineTo(x + 53.0f, y + 31.0f);
    tear.lineTo(x + 35.0f, y + 13.0f);
    tear.lineTo(x + 16.0f, y + 26.0f);
    tear.closeSubPath();

    g.setColour(paperPurple.withAlpha(0.15f + glow * 0.32f));
    g.fillPath(tear);
    g.setColour(paperInk.withAlpha(0.84f));
    g.strokePath(tear, juce::PathStrokeType(2.0f));
    drawLabel(g, "TEAR", { x + 70.0f, y + 50.0f }, 15.0f, paperInk);
    drawLabel(g, "rip / crack", { x + 70.0f, y + 69.0f }, 10.0f, paperInk.withAlpha(0.65f));
    drawIndicator(g, { x + 148.0f, y - 15.0f }, glow, paperPurple);
}

void PaperDrumsAudioProcessorEditor::drawCrumple(juce::Graphics& g,
                                                 juce::Rectangle<float> area,
                                                 float glow)
{
    const auto centre = area.getCentre();
    g.setColour(paperGreen.withAlpha(0.10f + glow * 0.28f));
    g.fillEllipse(area.reduced(area.getWidth() * 0.18f, area.getHeight() * 0.18f));
    g.setColour(paperInk.withAlpha(0.82f));

    for (int i = 0; i < 7; ++i)
    {
        juce::Path scribble;
        const auto dx = static_cast<float>((i % 3) - 1) * 18.0f;
        const auto dy = static_cast<float>((i / 3) - 1) * 14.0f;
        scribble.startNewSubPath(centre.x - 24.0f + dx, centre.y + 18.0f + dy);
        scribble.cubicTo(centre.x - 11.0f + dx, centre.y - 27.0f + dy,
                         centre.x + 17.0f + dx, centre.y + 28.0f + dy,
                         centre.x + 28.0f + dx, centre.y - 14.0f + dy);
        g.strokePath(scribble, juce::PathStrokeType(1.3f));
    }

    drawLabel(g, "CRUMPLE", { centre.x, area.getBottom() + 17.0f }, 14.0f, paperInk);
    drawIndicator(g, { area.getRight() - 2.0f, area.getY() + 12.0f }, glow, paperGreen);
}

void PaperDrumsAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1f2421));

    auto paper = getLocalBounds().toFloat().reduced(32.0f, 30.0f);
    g.setColour(juce::Colour(0x60000000));
    g.fillRoundedRectangle(paper.translated(5.0f, 9.0f), 15.0f);
    g.setColour(juce::Colour(0xfff1e4bc));
    g.fillRoundedRectangle(paper, 15.0f);

    // Slight paper grain / ruled construction marks.
    g.setColour(juce::Colour(0x153a3027));
    for (float y = paper.getY() + 90.0f; y < paper.getBottom() - 74.0f; y += 26.0f)
        g.drawLine(paper.getX() + 22.0f, y, paper.getRight() - 22.0f, y, 0.65f);

    g.setColour(juce::Colour(0x243a3027));
    g.drawLine(paper.getX() + 44.0f, paper.getY() + 24.0f, paper.getX() + 44.0f, paper.getBottom() - 20.0f, 1.0f);
    g.drawLine(paper.getRight() - 38.0f, paper.getY() + 20.0f, paper.getRight() - 38.0f, paper.getBottom() - 20.0f, 1.0f);

    drawLabel(g, "PAPER / DRUMS", { paper.getCentreX(), paper.getY() + 29.0f }, 22.0f, paperInk);
    drawLabel(g, "FOLEY SKETCH No. 01", { paper.getCentreX(), paper.getY() + 53.0f }, 10.0f,
              paperInk.withAlpha(0.62f));

    // Notebook holes.
    for (int i = 0; i < 6; ++i)
    {
        const auto y = paper.getY() + 105.0f + static_cast<float>(i) * 57.0f;
        g.setColour(juce::Colour(0x263a3027));
        g.fillEllipse(paper.getX() + 13.0f, y, 7.0f, 7.0f);
    }

    // Small hand-drawn arrows and notes.
    g.setColour(paperInk.withAlpha(0.45f));
    g.drawLine(paper.getX() + 86.0f, paper.getY() + 77.0f, paper.getX() + 136.0f, paper.getY() + 94.0f, 1.2f);
    g.drawLine(paper.getX() + 136.0f, paper.getY() + 94.0f, paper.getX() + 128.0f, paper.getY() + 87.0f, 1.2f);
    g.drawLine(paper.getX() + 136.0f, paper.getY() + 94.0f, paper.getX() + 126.0f, paper.getY() + 96.0f, 1.2f);
    drawLabel(g, "tap the ink", { paper.getX() + 110.0f, paper.getY() + 76.0f }, 9.0f,
              paperInk.withAlpha(0.56f));

    const auto kickGlow = processor.getPadActivity(PaperDrumsAudioProcessor::Kick);
    const auto snareGlow = processor.getPadActivity(PaperDrumsAudioProcessor::Snare);
    const auto hatGlow = processor.getPadActivity(PaperDrumsAudioProcessor::Hat);
    const auto tearGlow = processor.getPadActivity(PaperDrumsAudioProcessor::Tear);
    const auto crumpleGlow = processor.getPadActivity(PaperDrumsAudioProcessor::Crumple);

    drawTear(g, padAreas[PaperDrumsAudioProcessor::Tear], tearGlow);
    drawHat(g, padAreas[PaperDrumsAudioProcessor::Hat], hatGlow);
    drawCrumple(g, padAreas[PaperDrumsAudioProcessor::Crumple], crumpleGlow);
    drawKick(g, padAreas[PaperDrumsAudioProcessor::Kick], kickGlow);
    drawSnare(g, padAreas[PaperDrumsAudioProcessor::Snare], snareGlow);

    const auto footer = paper.getBottom() - 35.0f;
    drawLabel(g, "CLICK A SKETCH  •  OR PLAY MIDI  •  36  38  42  46  49", { paper.getCentreX(), footer }, 10.0f,
              paperInk.withAlpha(0.72f));
    g.setColour(paperInk.withAlpha(0.52f));
    g.drawLine(paper.getX() + 70.0f, footer + 20.0f, paper.getRight() - 70.0f, footer + 20.0f, 1.0f);
    drawLabel(g, "paper is the kit", { paper.getCentreX(), footer + 35.0f }, 10.0f, paperInk.withAlpha(0.60f));
}

int PaperDrumsAudioProcessorEditor::padAt(juce::Point<float> point) const
{
    for (int pad = 0; pad < PaperDrumsAudioProcessor::padCount; ++pad)
        if (padAreas[static_cast<size_t>(pad)].contains(point))
            return pad;

    return -1;
}

void PaperDrumsAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    const auto pad = padAt(event.position.toFloat());
    if (pad >= 0)
        processor.firePad(pad, event.mods.isShiftDown() ? 0.65f : 1.0f);
}

void PaperDrumsAudioProcessorEditor::timerCallback()
{
    processor.decayPadActivity(0.78f);
    repaint();
}
