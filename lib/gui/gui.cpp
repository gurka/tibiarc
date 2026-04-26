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

#include "gui.hpp"

#include <memory>

#include "canvas.hpp"
#include "common.hpp"
#include "state.hpp"

namespace trc {
namespace gui {

void Gui::Resize(int width, int height) {
    GuiCanvas = std::make_unique<Canvas>(width, height, Canvas::Type::External);
}

void Gui::Render(const State &state) {
    GuiCanvas->Wipe();
    for (const auto &widget : Widgets) {
        widget->Render(*GuiCanvas, state);
    }
}

void Gui::MouseLeftDown(int x, int y) {
    for (const auto &widget : Widgets) {
        if (MouseIsOverWidget(*widget, x, y)) {
            widget->MouseLeftDown(x, y);
        }
    }
}

void Gui::MouseLeftUp(int x, int y) {
    for (const auto &widget : Widgets) {
        if (MouseIsOverWidget(*widget, x, y)) {
            widget->MouseLeftUp(x, y);
        }
    }
}

} // namespace gui
} // namespace trc
