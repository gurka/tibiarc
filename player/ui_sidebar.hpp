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

#include <memory>

extern "C" {
#include <SDL.h>
}

#include "ui_common.hpp"
#include "canvas.hpp"
#include "renderer.hpp"
#include "gamestate.hpp"

namespace trc {

struct UiSidebar {
    SDL_Rect Rect;
    std::unique_ptr<Canvas> Canvas;
    UiCommon::Wrapper<SDL_Texture> Texture;

    void UpdateSize(SDL_Rect rect, SDL_Renderer *renderer);
    void Render(const Renderer::Options &renderOptions,
                const Gamestate &gamestate,
                SDL_Renderer *renderer) const;
    void MouseClick(int x, int y);

private:
    int DrawSidebarTop(const Gamestate &gamestate, trc::Canvas &canvas) const;

    void DrawMinimapArea(const Gamestate &gamestate,
                         trc::Canvas &canvas,
                         int &offsetY) const;
    void DrawStatusBars(const Gamestate &gamestate,
                        trc::Canvas &canvas,
                        int &offsetY) const;
    void DrawInventoryArea(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int &offsetY) const;
    void DrawWindowButtons(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int &offsetY) const;

    void DrawSidebarMiddle(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int &offsetY) const;

    void DrawSidebarWindowBackground(const Gamestate &gamestate,
                                     trc::Canvas &canvas,
                                     int offsetY,
                                     int height) const;
    void DrawSidebarWindow(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int offsetY,
                           const Sprite &icon,
                           const std::string &title,
                           int height) const;
    void DrawSkillsWindow(const Gamestate &gamestate,
                          trc::Canvas &canvas,
                          int &offsetY) const;
    void DrawBattleWindow(const Gamestate &gamestate,
                          trc::Canvas &canvas,
                          int &offsetY) const;

    void DrawSidebarBottom(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int offsetY) const;
};

} // namespace trc

#endif /* PLAYER_UI_SIDEBAR_HPP */
