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

#include "gui/panel.hpp"

#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"

#include "canvas.hpp"
#include "utils.hpp"

namespace trc {
namespace gui {

void Panel::Update(State &state, Position offset) {
    // Handle drag action
    if (Drag.Active()) {
        auto *pw = GetWidgetAndPosition(Drag.Target);
        AbortUnless(pw != nullptr);
        if (Drag.Update(state.MouseLeftDown(), state.MousePosition(offset), Width, Height)) {
            pw->Position = Drag.CurrentPosition;
        }
    }

    // Handle resize action
    if (Resize.Active()) {
        auto *pw = GetWidgetAndPosition(Resize.Target);
        AbortUnless(pw != nullptr);
        const auto maxHeightPanel = Height - pw->Position.Y;
        Resize.Update(state.MouseLeftDown(), state.MousePosition(offset), maxHeightPanel);
    }

    for (const auto &wap : Widgets) {
        auto &[widget, position] = wap;
        if (!widget->Visible) {
            continue;
        }
        widget->Update(state, offset + position);
    }
}

void Panel::Render(Canvas &canvas, Position offset) {
    for (const auto &wap : Widgets) {
        auto &[widget, position] = wap;
        if (!widget->Visible) {
            continue;
        }
        widget->Render(canvas, offset + position);
    }
}

Widget::MouseEventResult Panel::MouseLeftDown(Position position) {
    PressedWidget = nullptr;

    for (const auto &wap : Widgets) {
        if (!wap.Widget->Visible) {
            continue;
        }
        if (PointInsideWidget(position, wap)) {
            const auto result = wap.Widget->MouseLeftDown(position - wap.Position);
            if (result == Widget::MouseEventResult::StartDrag) {
                Drag.Begin(wap.Widget.get(), wap.Position, position);
            } else if (result == Widget::MouseEventResult::Resize) {
                Resize.Begin(wap.Widget.get(), position);
            }

            if (result != Widget::MouseEventResult::NotHandled) {
                PressedWidget = wap.Widget.get();
                PressedWidgetPosition = wap.Position;
            }

            return Widget::MouseEventResult::Handled;
        }
    }

    return Widget::MouseEventResult::NotHandled;
}

void Panel::MouseLeftUp(Position position) {
    if (PressedWidget != nullptr) {
        PressedWidget->MouseLeftUp(position - PressedWidgetPosition);
        PressedWidget = nullptr;
    }
}

PlacedWidget *Panel::GetWidgetAndPosition(Widget *widget) {
    for (auto &wap : Widgets) {
        if (wap.Widget.get() == widget) {
            return &wap;
        }
    }
    return nullptr;
}

} // namespace gui
} // namespace trc
