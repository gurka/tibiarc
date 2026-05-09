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

#include <algorithm>

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
      ResizeBottomWidget(false),
      DragTarget(nullptr),
      DragTargetPosition(0, 0),
      DragTargetInitialPosition(0, 0),
      DragMouseInitialPosition(0, 0),
      ResizeTarget(nullptr),
      ResizeTargetInitialHeight(0),
      ResizeMouseInitialPosition(0, 0) {
}

void VerticalPanel::Update(State &state, Position offset) {
    // Handle drag action
    if (DragTarget != nullptr) {
        if (!state.MouseLeftDown()) {
            DragTarget = nullptr;
        } else {
            DragTargetPosition = DragTargetInitialPosition +
                                 state.MousePosition(offset) -
                                 DragMouseInitialPosition;
            DragTargetPosition.X =
                    std::clamp(DragTargetPosition.X,
                               0,
                               Width - DragTarget->Width);
            DragTargetPosition.Y =
                    std::clamp(DragTargetPosition.Y,
                               0,
                               Height - DragTarget->Height);
        }
    }

    // Handle resize action
    if (ResizeTarget != nullptr) {
        if (!state.MouseLeftDown()) {
            ResizeTarget = nullptr;
        } else {
            const auto minHeight = ResizeTarget->MinHeight; 
            const auto maxHeight = std::min(ResizeTarget->MaxHeight,
                                            Height - GetWidgetY(ResizeTarget));
            ResizeTarget->Height = std::clamp(
                    ResizeTargetInitialHeight + state.MousePosition(offset).Y -
                            ResizeMouseInitialPosition.Y,
                    minHeight,
                    maxHeight);
        }
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
    if (DragTarget != nullptr) {
        // Find the widget that the drag target is intersecting with (if any)
        Widget *otherWidget = nullptr;
        auto otherY = 0;
        bool otherIsAbove = true;
        for (auto &widget : Widgets) {
            if (widget.get() == DragTarget) {
                otherIsAbove = false;
            } else if (AreasIntersect(DragTargetPosition,
                                      DragTarget->Width,
                                      DragTarget->Height,
                                      Position(0, otherY),
                                      widget->Width,
                                      widget->Height)) {
                otherWidget = widget.get();
                break;
            }
            otherY += widget->Height;
        }

        if (otherWidget != nullptr) {
            const auto otherMidY = otherY + (otherWidget->Height / 2);
            if ((otherIsAbove && DragTargetPosition.Y < otherMidY) ||
                (!otherIsAbove &&
                  DragTargetPosition.Y + DragTarget->Height > otherMidY)) {
                std::swap(Widgets[GetWidgetIndex(DragTarget)],
                          Widgets[GetWidgetIndex(otherWidget)]);
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
        if (widget.get() != DragTarget) {
            widget->Render(canvas, offset + gui::Position(0, y));
        }
        y += widget->Height;
    }
    if (DragTarget != nullptr) {
        DragTarget->Render(canvas, offset + DragTargetPosition);
    }
}

Widget::MouseEventResult VerticalPanel::MouseLeftDown(Position position) {
    auto y = 0;
    for (const auto &widget : Widgets) {
        if (PointInsideArea(position,
                            Position(0, y),
                            widget->Width,
                            widget->Height)) {
            const auto result = widget->MouseLeftDown(position - Position(0, y));
            if (result == Widget::MouseEventResult::StartDrag) {
                DragTarget = widget.get();
                DragTargetPosition = Position(0, y);
                DragTargetInitialPosition = DragTargetPosition;
                DragMouseInitialPosition = position;
            } else if (result == Widget::MouseEventResult::Resize) {
                ResizeTarget = widget.get();
                ResizeTargetInitialHeight = widget->Height;
                ResizeMouseInitialPosition = position;
            }

            return Widget::MouseEventResult::None;
        }

        y += widget->Height;
    }

    return Widget::MouseEventResult::None;
}

void VerticalPanel::MouseLeftUp(Position position) {
    auto y = 0;
    for (const auto &widget : Widgets) {
        if (PointInsideArea(position, Position(0, y), widget->Width, widget->Height)) {
            widget->MouseLeftUp(position - Position(0, y));
        }
        y += widget->Height;
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
