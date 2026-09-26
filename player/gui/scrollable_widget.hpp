/*
 * Copyright 2026 "Simon Sandström"
 *
 * This file is part of tibiarc.
 *
 * tibiarc is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * tibiarc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with tibiarc. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __TRC_GUI_SCROLLABLE_WIDGET_HPP__
#define __TRC_GUI_SCROLLABLE_WIDGET_HPP__

#include "gui/button.hpp"
#include "gui/position.hpp"
#include "gui/widget.hpp"

namespace trc {

class Canvas;
struct Version;

namespace gui {

struct State;

// Base class providing a reusable vertical scrollbar: a pair of
// scroll-up/scroll-down buttons plus a draggable, wheel- and
// click-scrollable thumb/track. Deriving widgets supply the geometry
// (button positions, track position/size) and the content/viewport sizes;
// this class owns the scroll offset and all interaction/rendering logic.
struct ScrollableWidget : public Widget {
protected:
    ScrollableWidget(int width, int height, const Version *version);

    // Total height (in px) of the scrollable content.
    virtual int GetScrollContentHeight() const = 0;

    // Height (in px) of the viewport through which content is visible.
    virtual int GetScrollViewportHeight() const = 0;

    // Geometry hooks, all relative to the widget's own offset.
    virtual Position GetScrollUpButtonPosition() const = 0;
    virtual Position GetScrollDownButtonPosition() const = 0;
    virtual int GetScrollbarTrackTop() const = 0;
    virtual int GetScrollbarTrackHeight() const = 0;
    virtual int GetScrollbarX() const = 0;

    int ScrollOffset = 0;
    Button ScrollUpButton;
    Button ScrollDownButton;

    int MaxScrollOffset() const;
    void ClampScrollOffset();
    bool CanScroll() const;
    bool GetScrollbarThumbMetrics(int &thumbOffset, int &thumbHeight) const;

    // Cancels any in-progress thumb drag, e.g. when content is hidden/resized.
    void CancelScrollbarDrag();

    // Call from the derived widget's Update().
    void UpdateScrollbar(State &state, Position offset);

    // Call from the derived widget's Render() to draw the up/down buttons,
    // track background and thumb.
    void RenderScrollbar(Canvas &canvas, Position offset);

    // Call from the derived widget's OnMouseEvent(). Returns NotHandled if
    // the event was not related to the scrollbar (e.g. an unrelated wheel
    // event when there's nothing to scroll).
    MouseEventResult OnScrollbarMouseEvent(MouseEvent event, Position position);

private:
    const Version *_Version;

    bool ScrollbarThumbPressed = false;
    int ScrollbarThumbDragOffsetY = 0;
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_SCROLLABLE_WIDGET_HPP__
