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
        None,        // Event was not handled
        Clicked,     // Event was handled and resulted in a click action
        StartDrag,   // Event was handled and started a drag action
        Resize,      // Event was handled and started a resize action
    };

    int Width;
    int Height;

    Widget(int width, int height)
        : Width(width), Height(height) {
    }

    virtual void Update(State &state, Position offset) {
    }
    virtual void Render(Canvas &canvas, Position offset) = 0;

    virtual MouseEventResult MouseLeftDown(Position position) {
        return MouseEventResult::None;
    }
    virtual void MouseLeftUp(Position position) {
    }
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_WIDGET_HPP__
