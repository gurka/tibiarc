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

Panel::~Panel() = default;

bool Panel::SetChildPosition(Widget *widget, Position position) {
    auto *pw = GetWidgetAndPosition(widget);
    if (pw == nullptr) {
        return false;
    }
    pw->Position = position;
    return true;
}

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
    if (Background) {
        if (!CachedBackground) {
            CachedBackground = std::make_unique<Canvas>(Width, Height);
            CachedBackground->DrawTiled(*Background,
                                        0,
                                        0,
                                        Width,
                                        Height);
        }

        Canvas::Copy(canvas,
                     *CachedBackground,
                     0,
                     0,
                     CachedBackground->Width,
                     CachedBackground->Height,
                     offset.X,
                     offset.Y);
    }
    for (const auto &wap : Widgets) {
        auto &[widget, position] = wap;
        if (!widget->Visible) {
            continue;
        }
        widget->Render(canvas, offset + position);
    }
}

Widget::MouseEventResult Panel::OnMouseEvent(MouseEvent event, Position position) {
    if (event == MouseEvent::LeftDown) {
        PressedWidget = nullptr;

        for (const auto &wap : Widgets) {
            if (!wap.Widget->Visible) {
                continue;
            }
            if (PointInsideWidget(position, wap)) {
                const auto result = wap.Widget->OnMouseEvent(event, position - wap.Position);
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
    } else if (event == MouseEvent::LeftUp) {
        if (PressedWidget != nullptr) {
            PressedWidget->OnMouseEvent(event, position - PressedWidgetPosition);
            PressedWidget = nullptr;
        }
        return Widget::MouseEventResult::Handled;
    } else {
        // WheelUp / WheelDown
        for (const auto &wap : Widgets) {
            if (!wap.Widget->Visible) {
                continue;
            }
            if (PointInsideWidget(position, wap)) {
                return wap.Widget->OnMouseEvent(event, position - wap.Position);
            }
        }
        return Widget::MouseEventResult::NotHandled;
    }
}

PlacedWidget<> *Panel::GetWidgetAndPosition(Widget *widget) {
    for (auto &wap : Widgets) {
        if (wap.Widget.get() == widget) {
            return &wap;
        }
    }
    return nullptr;
}

void Panel::SetLayoutSize(int width, int height) {
    Widget::SetLayoutSize(width, height);
    CachedBackground.reset();
}

} // namespace gui
} // namespace trc
