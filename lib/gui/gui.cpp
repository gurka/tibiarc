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

#include "gui/gui.hpp"

#include <memory>
#include <tuple>

#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"

#include "canvas.hpp"

namespace trc {
namespace gui {

void Gui::Resize(int width, int height) {
    GuiCanvas = std::make_unique<Canvas>(width, height, Canvas::Type::External);
}

void Gui::Render(const State &state) {
    GuiCanvas->Wipe();
    for (const auto &wap : Widgets) {
        std::get<0>(wap)->Render(state, *GuiCanvas, std::get<1>(wap));
    }
}

void Gui::MouseLeftDown(Position position) {
    for (const auto &wap : Widgets) {
        if (PointInsideWidget(position, wap)) {
            std::get<0>(wap)->MouseLeftDown(position - std::get<1>(wap));
        }
    }
}

void Gui::MouseLeftUp(Position position) {
    for (const auto &wap : Widgets) {
        if (PointInsideWidget(position, wap)) {
            std::get<0>(wap)->MouseLeftUp(position - std::get<1>(wap));
        }
    }
}

} // namespace gui
} // namespace trc
