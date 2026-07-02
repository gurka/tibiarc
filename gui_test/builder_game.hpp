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

#ifndef __TRC_GUI_TEST_GUI_BUILDER_GAME_HPP__
#define __TRC_GUI_TEST_GUI_BUILDER_GAME_HPP__

#include <memory>

#include "state.hpp"

#include "gui/widget.hpp"
#include "gamestate.hpp"

namespace Builder {

std::unique_ptr<trc::gui::Widget> BuildGame(int windowWidth, int windowHeight, trc::Gamestate *gamestate, GuiState *guiState);

} // namespace Builder

#endif // __TRC_GUI_TEST_GUI_BUILDER_GAME_HPP__
