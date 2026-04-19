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

#ifndef PLAYER_UI_SIDEBAR_HPP_
#define PLAYER_UI_SIDEBAR_HPP_

#include <string>

#include "gamestate.hpp"
#include "canvas.hpp"
#include "sprites.hpp"

namespace trc {
namespace UiSidebar {

int DrawSidebarTop(Gamestate &gamestate, Canvas &canvas) noexcept;

void DrawMinimapArea(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept;
void DrawStatusBars(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept;
void DrawInventoryArea(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept;
void DrawWindowButtons(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept;

void DrawSidebarMiddle(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept;

void DrawSidebarWindowBackground(Gamestate &gamestate,
                                 Canvas &canvas,
                                 int offsetY,
                                 int height) noexcept;
void DrawSidebarWindow(Gamestate &gamestate,
                       Canvas &canvas,
                       int offsetY,
                       const Sprite &icon,
                       const std::string &title,
                       int height) noexcept;
void DrawSkillsWindow(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept;
void DrawBattleWindow(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept;

void DrawSidebarBottom(Gamestate &gamestate,
                       Canvas &canvas,
                       int offsetY) noexcept;

} // namespace UiSidebar
} // namespace trc

#endif /* PLAYER_UI_SIDEBAR_HPP */
