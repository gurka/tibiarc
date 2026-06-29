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

#include "gui/vertical_panel.hpp"

#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"
#include "gui/widget.hpp"

#include "canvas.hpp"

namespace trc {
namespace gui {

VerticalPanel::VerticalPanel(int width, int height)
    : Widget(width, height),
      Widgets(),
      DynamicHeight(false),
      ResizeBottomWidget(false) {
}

void VerticalPanel::Update(State &state, Position offset) {
    // Handle drag action
    if (Drag.Active()) {
        Drag.Update(state.MouseLeftDown(), state.MousePosition(offset), Width, Height);
    }

    // Handle resize action
    if (Resize.Active()) {
        const auto maxHeightPanel = Height - GetWidgetY(Resize.Target);
        Resize.Update(state.MouseLeftDown(), state.MousePosition(offset), maxHeightPanel);
    }

    // Update widgets
    auto y = 0;
    for (auto i = 0; i < Widgets.size(); ++i) {
        auto &widget = Widgets[i];
        if (ResizeBottomWidget && i == Widgets.size() - 1) {
            widget->Height = Height - y;
        }
        widget->Update(state, offset + Position(0, y));
        y += widget->Height;
    }

    // If a widget is being dragged, check if we need to shift any
    // other widget up or down
    if (Drag.Active()) {
        // Find the widget that the drag target is intersecting with (if any)
        Widget *otherWidget = nullptr;
        auto otherY = 0;
        bool otherIsAbove = true;
        for (auto &widget : Widgets) {
            if (widget.get() == Drag.Target) {
                otherIsAbove = false;
            } else if (AreasIntersect(Drag.CurrentPosition,
                                      Drag.Target->Width,
                                      Drag.Target->Height,
                                      Position(0, otherY),
                                      widget->Width,
                                      widget->Height)) {
                otherWidget = widget.get();
                break;
            }
            otherY += widget->Height;
        }

        if (otherWidget != nullptr) {
            // Don't swap with the bottom widget if ResizeBottomWidget is true
            if (!ResizeBottomWidget ||
                otherWidget != Widgets[Widgets.size() - 1].get()) {
                const auto otherMidY = otherY + (otherWidget->Height / 2);
                if ((otherIsAbove && Drag.CurrentPosition.Y < otherMidY) ||
                    (!otherIsAbove &&
                     Drag.CurrentPosition.Y + Drag.Target->Height > otherMidY)) {
                    std::swap(Widgets[GetWidgetIndex(Drag.Target)],
                              Widgets[GetWidgetIndex(otherWidget)]);
                }
            }
        }
    }

    if (DynamicHeight) {
        // Make sure that Height is up to date
        // TODO: What can we do to avoid having to recalculate this every frame?
        Height = 0;
        for (const auto &widget : Widgets) {
            Height += widget->Height;
        }
    }
}

void VerticalPanel::Render(Canvas &canvas, Position offset) {
    auto y = 0;
    for (const auto &widget : Widgets) {
        if (widget.get() != Drag.Target) {
            widget->Render(canvas, offset + gui::Position(0, y));
        }
        y += widget->Height;
    }
    if (Drag.Active()) {
        Drag.Target->Render(canvas, offset + Drag.CurrentPosition);
    }
}

Widget::MouseEventResult VerticalPanel::MouseLeftDown(Position position) {
    PressedWidget = nullptr;

    auto y = 0;
    for (const auto &widget : Widgets) {
        if (PointInsideArea(position,
                            Position(0, y),
                            widget->Width,
                            widget->Height)) {
            const auto result = widget->MouseLeftDown(position - Position(0, y));
            if (result == Widget::MouseEventResult::StartDrag) {
                Drag.Begin(widget.get(), Position(0, y), position);
            } else if (result == Widget::MouseEventResult::Resize) {
                Resize.Begin(widget.get(), position);
            }

            if (result != Widget::MouseEventResult::NotHandled) {
                PressedWidget = widget.get();
                PressedWidgetY = y;
            }

            return Widget::MouseEventResult::Handled;
        }

        y += widget->Height;
    }

    return Widget::MouseEventResult::NotHandled;
}

void VerticalPanel::MouseLeftUp(Position position) {
    if (PressedWidget != nullptr) {
        PressedWidget->MouseLeftUp(position - Position(0, PressedWidgetY));
        PressedWidget = nullptr;
    }
}

int VerticalPanel::GetWidgetY(const Widget *widget) const {
    int y = 0;
    for (const auto &w : Widgets) {
        if (w.get() == widget) {
            return y;
        }
        y += w->Height;
    }
    std::terminate();
}

int VerticalPanel::GetWidgetIndex(const Widget *widget) const {
    for (auto i = 0; i < Widgets.size(); ++i) {
        if (Widgets[i].get() == widget) {
            return i;
        }
    }
    std::terminate();
}

} // namespace gui
} // namespace trc
