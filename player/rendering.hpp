/*
 * Copyright 2024-2026 "Simon Sandström"
 * Copyright 2024 "John Högberg"
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

#ifndef PLAYER_RENDERING_H
#define PLAYER_RENDERING_H

extern "C" {
#include <SDL.h>
}

#include "playback.hpp"
#include "ui_sidebar.hpp"
#include "ui_chat.hpp"
#include "ui_game.hpp"

struct SDL_Window;
struct SDL_Renderer;

namespace trc {

struct Rendering {
    UiCommon::Wrapper<SDL_Window> Window;
    UiCommon::Wrapper<SDL_Renderer> Renderer;

    Renderer::Options RenderOptions;

    UiGame Game;
    UiSidebar Sidebar;
    UiChat Chat;

    uint32_t StatsLastUpdate = 0;
    uint32_t StatsFramesSinceLastUpdate = 0;
    double StatsFPS = 1.0;

    Rendering(int width, int height);

    void HandleResize();
    void Render(Playback &playback);
    void MouseClick(int x, int y);
};
} // namespace trc

#endif /* PLAYER_RENDERING_H */
