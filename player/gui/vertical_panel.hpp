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

#ifndef __TRC_GUI_VERTICAL_PANEL_HPP__
#define __TRC_GUI_VERTICAL_PANEL_HPP__

#include <memory>
#include <vector>

#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/widget.hpp"

namespace trc {

class Canvas;

namespace gui {

struct State;

struct VerticalPanel : public Widget {
    VerticalPanel(int width, int height);

    // Whether the panel should automatically adjust its height based on the height
    // of all widgets
    void SetDynamicHeight(bool dynamicHeight) { DynamicHeight = dynamicHeight; }

    // Whether the last widget should be stretched to fill the remaining
    // space in the panel
    void SetStretchLastChild(bool stretchLastChild) { StretchLastChild = stretchLastChild; }

    template<typename T>
    T &Add(std::unique_ptr<T> widget) {
        T &ref = *widget;
        Widgets.emplace_back(std::move(widget));
        return ref;
    }

    bool Remove(Widget *widget);

    void SetWidth(int w) override;

    void Update(State &state, Position offset) override;
    void Render(Canvas &canvas, Position offset) override;

    MouseEventResult OnMouseEvent(MouseEvent event, Position position) override;

private:
    bool DynamicHeight;
    bool StretchLastChild;

    std::vector<std::unique_ptr<Widget>> Widgets;

    DragState Drag;
    ResizeState Resize;

    Widget *PressedWidget = nullptr;
    int PressedWidgetY = 0;

    int GetWidgetY(const Widget *widget) const;
    int GetWidgetIndex(const Widget *widget) const;
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_VERTICAL_PANEL_HPP__

