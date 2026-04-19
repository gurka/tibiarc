/*
 * Copyright 2024 "Simon Sandström"
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
#include "renderer.hpp"
#include "canvas.hpp"

#include <functional>
#include <memory>

namespace trc {
struct Rendering {
    template <typename T>
    using Wrapper = std::unique_ptr<T, std::function<void(T *)>>;

    Wrapper<SDL_Window> Window;
    Wrapper<SDL_Renderer> Renderer;

    Renderer::Options RenderOptions;

    // The window consists of three areas:
    // - Game
    // - Sidebar
    // - Chat
    // 
    // Sidebar's size is always 176 x <window height>
    // Chat's height is adjustable, but its width is always <window width> - 176
    // Game's height depends on the chat's height, but its width is always <window width> - 176
    struct Area {
        SDL_Rect Rect;
        std::unique_ptr<Canvas> Canvas;
        Wrapper<SDL_Texture> Texture;
    };

    Area Game;
    Area Sidebar;
    Area Chat;

    // The gamestate is always rendered in NATIVE_RESOLUTION
    // and then copied to the game texture (scaled but with the same ratio)
    // Also, Renderer::DrawOverlay wants a canvas with the same size
    // as Gamestate after scaling, so give it that
    std::unique_ptr<Canvas> CanvasGamestate;
    Wrapper<SDL_Texture> TextureGamestate;
    std::unique_ptr<Canvas> CanvasOverlay;
    Wrapper<SDL_Texture> TextureOverlay;
    SDL_Rect RectGamestate;

    uint32_t StatsLastUpdate = 0;
    uint32_t StatsFramesSinceLastUpdate = 0;
    double StatsFPS = 1.0;

    Rendering(int width, int height);

    void HandleResize();
    void Render(Playback &playback);

private:
    Wrapper<SDL_Texture> CreateTexture(int width, int height) const;
    void ResetArea(Area &area, int x, int y, int w, int h) const;
};
} // namespace trc

#endif /* PLAYER_RENDERING_H */
