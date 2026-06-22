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

#include <algorithm>

#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"

#include "canvas.hpp"
#include "utils.hpp"

namespace trc {
namespace gui {

void Panel::Update(State &state, Position offset) {
    // Handle drag action
    if (DragTarget != nullptr) {
        if (!state.MouseLeftDown()) {
            DragTarget = nullptr;
        } else {
            auto *wap = GetWidgetAndPosition(DragTarget);
            AbortUnless(wap != nullptr);

#pragma warning(suppress : 6011)
            wap->Position = DragTargetInitialPosition +
                            state.MousePosition(offset) -
                            DragMouseInitialPosition;
            wap->Position.X = std::clamp(wap->Position.X,
                                         0,
                                         Width - wap->Widget->Width);
            wap->Position.Y = std::clamp(wap->Position.Y,
                                         0,
                                         Height - wap->Widget->Height);
        }
    }

    // Handle resize action
    if (ResizeTarget != nullptr) {
        if (!state.MouseLeftDown()) {
            ResizeTarget = nullptr;
        } else {
            auto *wap = GetWidgetAndPosition(ResizeTarget);
            AbortUnless(wap != nullptr);

#pragma warning(suppress : 6011)
            const auto maxHeightPanel = Height - wap->Position.Y;
            ResizeTarget->Height = std::clamp(
                    ResizeTargetInitialHeight + state.MousePosition(offset).Y -
                            ResizeMouseInitialPosition.Y,
                    ResizeTarget->MinHeight,
                    std::min(ResizeTarget->MaxHeight, maxHeightPanel));
        }
    }

    for (const auto &wap : Widgets) {
        auto &[widget, position] = wap;
        widget->Update(state, offset + position);
    }
}

void Panel::Render(Canvas &canvas, Position offset) {
    for (const auto &wap : Widgets) {
        auto &[widget, position] = wap;
        widget->Render(canvas, offset + position);
    }
}

Widget::MouseEventResult Panel::MouseLeftDown(Position position) {
    for (const auto &wap : Widgets) {
        if (PointInsideWidget(position, wap)) {
            const auto result = wap.Widget->MouseLeftDown(position - wap.Position);
            if (result == Widget::MouseEventResult::StartDrag) {
                DragTarget = wap.Widget.get();
                DragTargetInitialPosition = wap.Position;
                DragMouseInitialPosition = position;
            } else if (result == Widget::MouseEventResult::Resize) {
                ResizeTarget = wap.Widget.get();
                ResizeTargetInitialHeight = wap.Widget->Height;
                ResizeMouseInitialPosition = position;
            }

            return Widget::MouseEventResult::Handled;
        }
    }

    return Widget::MouseEventResult::NotHandled;
}

void Panel::MouseLeftUp(Position position) {
    for (const auto &wap : Widgets) {
        if (PointInsideWidget(position, wap)) {
            wap.Widget->MouseLeftUp(position - wap.Position);
        }
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
