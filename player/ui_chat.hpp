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

#ifndef PLAYER_UI_CHAT_HPP_
#define PLAYER_UI_CHAT_HPP_

#include "gamestate.hpp"
#include "canvas.hpp"

namespace trc {
namespace UiChat {

void DrawChat(Gamestate &gamestate, Canvas &canvas) noexcept;

} // namespace UiChat
} // namespace trc

#endif /* PLAYER_UI_CHAT_HPP */
