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

#ifndef __TRC_GUI_PANEL_HPP__
#define __TRC_GUI_PANEL_HPP__

#include <vector>

#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/widget.hpp"

namespace trc {

class Canvas;

namespace gui {

struct State;

struct Panel : public Widget {
    Panel(int width, int height)
        : Widget(width, height) {
    }

    template<typename T>
    T &Add(std::unique_ptr<T> widget, Position position) {
        T &ref = *widget;
        Widgets.push_back({std::move(widget), position});
        return ref;
    }

    void Update(State &state, Position offset) override;
    void Render(Canvas &canvas, Position offset) override;

    MouseEventResult MouseLeftDown(Position position) override;
    void MouseLeftUp(Position position) override;

private:
    std::vector<PlacedWidget> Widgets;

    DragState Drag;
    ResizeState Resize;

    PlacedWidget *GetWidgetAndPosition(Widget *widget);
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_PANEL_HPP__

