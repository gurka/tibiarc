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

namespace trc {
namespace gui {

void Panel::Render(const State &state,
                   Canvas &canvas,
                   Position offset) {
    // Handle drag action
    if (DragTarget != nullptr) {
        Position mouseDelta = state.MousePosition() - DragMouseInitialPosition;
        for (auto &wap : Widgets) {
            if (std::get<0>(wap).get() == DragTarget) {
                std::get<1>(wap) = DragTargetInitialPosition + mouseDelta;
                std::get<1>(wap).X =
                        std::clamp(std::get<1>(wap).X,
                                   0,
                                   Width - std::get<0>(wap)->Width);
                std::get<1>(wap).Y =
                        std::clamp(std::get<1>(wap).Y,
                                   0,
                                   Height - std::get<0>(wap)->Height);
                break;
            }
        }

        if (!state.MouseLeftDown()) {
            DragTarget = nullptr;
        }
    }

    // Render
    for (const auto &wap : Widgets) {
        std::get<0>(wap)->Render(state, canvas, offset + std::get<1>(wap));
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

} // namespace gui
} // namespace trc
