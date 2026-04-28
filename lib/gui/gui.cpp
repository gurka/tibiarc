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

#include "gui/panel.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"

#include "canvas.hpp"

namespace trc {
namespace gui {

void Gui::Resize(int width, int height) {
    GuiCanvas = std::make_unique<Canvas>(width, height, Canvas::Type::External);
    RootPanel = std::make_unique<Panel>(width, height);
}

void Gui::AddWidget(std::unique_ptr<Widget> &&widget, Position position) {
    RootPanel->Widgets.push_back(std::make_tuple(std::move(widget), position));
}

void Gui::Render(const State &state) {
    GuiCanvas->Wipe();
    RootPanel->Render(state, *GuiCanvas, Position(0, 0));
}

void Gui::MouseLeftDown(Position position) {
    RootPanel->MouseLeftDown(position);
}

void Gui::MouseLeftUp(Position position) {
    RootPanel->MouseLeftUp(position);
}

} // namespace gui
} // namespace trc
