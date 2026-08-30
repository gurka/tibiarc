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

#ifndef __TRC_GUI_WIDGET_HPP__
#define __TRC_GUI_WIDGET_HPP__

#include "gui/position.hpp"

namespace trc {

class Canvas;

namespace gui {

struct State;

struct Widget {
    enum class MouseEventResult {
        NotHandled, // Event was not handled — caller may continue processing
        Handled,    // Event was handled — caller should stop processing
        StartDrag,  // Event was handled and started a drag action
        Resize,     // Event was handled and started a resize action
    };

    enum class MouseEvent {
        LeftDown,
        LeftUp,
        WheelUp,
        WheelDown,
    };

    int Width;
    int Height;

    // Only used if the widget is resizable, otherwise they should be equal
    // to Height
    int MinHeight;
    int MaxHeight;

    bool Visible = true;

    Widget(int width, int height)
        : Width(width), Height(height), MinHeight(height), MaxHeight(height) {
    }

    virtual ~Widget() = default; 

    // Called by a parent/container to assign this widget a specific width.
    // Wrapper widgets (e.g. Border) should override this to propagate the
    // assignment to their child.
    virtual void SetWidth(int w) {
        Width = w;
    }

    // Called by a parent/container to assign this widget a specific height.
    // Wrapper widgets (e.g. Border) should override this to propagate the
    // assignment to their child.
    virtual void SetHeight(int h) {
        Height = h;
    }

    virtual void SetSize(int w, int h) {
        SetWidth(w);
        SetHeight(h);
    }

    virtual void Update(State &state, Position offset) {
    }
    virtual void Render(Canvas &canvas, Position offset) = 0;

    virtual MouseEventResult OnMouseEvent(MouseEvent event, Position position) {
        return MouseEventResult::NotHandled;
    }
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_WIDGET_HPP__
