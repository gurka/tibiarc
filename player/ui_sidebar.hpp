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

    void UpdateSize(SDL_Rect rect, SDL_Renderer *renderer);
    void Render(const Renderer::Options &renderOptions,
                const Gamestate &gamestate,
                SDL_Renderer *renderer);
    void MouseClick(int x, int y);

private:
    std::unique_ptr<Canvas> Canvas;
    UiCommon::Wrapper<SDL_Texture> Texture;

    SDL_Rect ButtonResizeInventory;
    SDL_Rect ButtonSkills;
    SDL_Rect ButtonBattle;
    SDL_Rect ButtonVip;
    bool InventoryMinimized;
    bool SkillsShown;
    bool BattleShown;
    bool VipShown;

    int DrawSidebarTop(const Gamestate &gamestate, trc::Canvas &canvas);

    void DrawMinimapArea(const Gamestate &gamestate,
                         trc::Canvas &canvas,
                         int &offsetY);
    void DrawStatusBars(const Gamestate &gamestate,
                        trc::Canvas &canvas,
                        int &offsetY);
    void DrawInventoryArea(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int &offsetY);
    void DrawWindowButtons(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int &offsetY);

    void DrawSidebarMiddle(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int &offsetY);

    void DrawSidebarWindowBackground(const Gamestate &gamestate,
                                     trc::Canvas &canvas,
                                     int offsetY,
                                     int height);
    void DrawSidebarWindow(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int offsetY,
                           const Sprite &icon,
                           const std::string &title,
                           int height);
    void DrawSkillsWindow(const Gamestate &gamestate,
                          trc::Canvas &canvas,
                          int &offsetY);
    void DrawBattleWindow(const Gamestate &gamestate,
                          trc::Canvas &canvas,
                          int &offsetY);

    void DrawSidebarBottom(const Gamestate &gamestate,
                           trc::Canvas &canvas,
                           int offsetY);
};

} // namespace trc

#endif /* PLAYER_UI_SIDEBAR_HPP */
