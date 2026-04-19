/*
 * Copyright 2011-2016 "Silver Squirrel Software Handelsbolag"
 * Copyright 2023-2024 "John Högberg"
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

#ifndef __TRC_UI_RENDERER_HPP__
#define __TRC_UI_RENDERER_HPP__

#include <string>

#include "icons.hpp"
#include "canvas.hpp"
#include "gamestate.hpp"
#include "container.hpp"
#include "sprites.hpp"

namespace trc {
namespace UiRenderer {

/* FIXME: C++ migration, `noexcept` specifiers are there as a shorthand to
 * std::terminate() on data errors, which should've been caught by the
 * parser. */

// Note: this draws a "sunken" border, i.e. top and left lines are dark while
//       bottom and right lines are light
void DrawBorder1px(const Icons &icons,
                   Canvas &canvas,
                   int leftX,
                   int topY,
                   int rightX,
                   int bottomY) noexcept;

// Note: this draws a "raised" border, i.e. top and left lines are light while
//       bottom and right lines are dark
void DrawBorder2px(const Icons &icons,
                   Canvas &canvas,
                   int leftX,
                   int topY,
                   int rightX,
                   int bottomY) noexcept;

// Sidebar
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

// Chat
void DrawChat(Gamestate &gamestate, Canvas &canvas) noexcept;

// Other
void DrawIconBar(Gamestate &gamestate,
                 Canvas &canvas,
                 int &offsetX,
                 int &offsetY) noexcept;

int MeasureContainerHeight(Gamestate &gamestate,
                           Container &container,
                           bool collapsed,
                           int width);

void DrawContainer(Gamestate &gamestate,
                   Canvas &canvas,
                   Container &container,
                   bool collapsed,
                   int maxX,
                   int maxY,
                   int &offsetX,
                   int &offsetY) noexcept;

// TODO: Rename, as we draw more than backgrounds with it
void DrawBackground(const Sprite &sprite,
                    Canvas &canvas,
                    int topX,
                    int topY,
                    int rightX,
                    int rightY) noexcept;

} // namespace UiRenderer
} // namespace trc

#endif /* __TRC_UI_RENDERER_HPP__ */
