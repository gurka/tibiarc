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
#include <tuple>

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
        auto *wap = GetWidgetAndPosition(DragTarget);
        AbortUnless(wap != nullptr);

        #pragma warning(suppress: 6011)
        std::get<1>(*wap) = DragTargetInitialPosition + state.MousePosition(offset) - DragMouseInitialPosition;
        std::get<1>(*wap).X =
                std::clamp(std::get<1>(*wap).X,
                           Border,
                           Width - std::get<0>(*wap)->Width - Border);
        std::get<1>(*wap).Y =
                std::clamp(std::get<1>(*wap).Y,
                           Border,
                           Height - std::get<0>(*wap)->Height - Border);
        if (!state.MouseLeftDown()) {
            DragTarget = nullptr;
        }
    }

    // Handle resize action
    if (ResizeTarget != nullptr) {
        auto *wap = GetWidgetAndPosition(ResizeTarget);
        AbortUnless(wap != nullptr);

        #pragma warning(suppress: 6011)
        const auto maxHeight = Height - std::get<1>(*wap).Y - Border;
        // TODO: minHeight should probably be decided by the widget itself
        const auto minHeight = 50;
        ResizeTarget->Height = std::clamp(ResizeTargetInitialHeight + state.MousePosition(offset).Y - ResizeMouseInitialPosition.Y, minHeight, maxHeight);
        if (!state.MouseLeftDown()) {
            ResizeTarget = nullptr;
        }
    }

    for (const auto &wap : Widgets) {
        std::get<0>(wap)->Update(state, offset + std::get<1>(wap));
    }
}

void Panel::Render(Canvas &canvas, Position offset) {
    for (const auto &wap : Widgets) {
        std::get<0>(wap)->Render(canvas, offset + std::get<1>(wap));
    }
}

Widget::MouseEventResult Panel::MouseLeftDown(Position position) {
    for (const auto &wap : Widgets) {
        if (PointInsideWidget(position, wap)) {
            const auto result = std::get<0>(wap)->MouseLeftDown(position - std::get<1>(wap));
            if (result == Widget::MouseEventResult::StartDrag) {
                DragTarget = std::get<0>(wap).get();
                DragTargetInitialPosition = std::get<1>(wap);
                DragMouseInitialPosition = position;
            } else if (result == Widget::MouseEventResult::Resize) {
                ResizeTarget = std::get<0>(wap).get();
                ResizeTargetInitialHeight = std::get<0>(wap)->Height;
                ResizeMouseInitialPosition = position;
            }

            return Widget::MouseEventResult::None;
        }
    }

    return Widget::MouseEventResult::None;
}

void Panel::MouseLeftUp(Position position) {
    for (const auto &wap : Widgets) {
        if (PointInsideWidget(position, wap)) {
            std::get<0>(wap)->MouseLeftUp(position - std::get<1>(wap));
        }
    }
}

WidgetAndPosition *Panel::GetWidgetAndPosition(Widget *widget) {
    for (auto &wap : Widgets) {
        if (std::get<0>(wap).get() == widget) {
            return &wap;
        }
    }
    return nullptr;
}

} // namespace gui
} // namespace trc
