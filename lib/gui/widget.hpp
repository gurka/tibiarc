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

#include "canvas.hpp"
#include "state.hpp"

namespace trc {
namespace gui {

struct Widget {
    int X;
    int Y;
    int Width;
    int Height;

    Widget(int x, int y, int width, int height)
        : X(x), Y(y), Width(width), Height(height) {
    }

    virtual void Render(Canvas &canvas, const State &state) = 0;

    virtual void MouseLeftDown(int x, int y) {
    }
    virtual void MouseLeftUp(int x, int y) {
    }
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_WIDGET_HPP__
