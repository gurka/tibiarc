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

#include "rendering.hpp"

#include <iostream>

#include "canvas.hpp"
#include "versions.hpp"
#include "textrenderer.hpp"
#include "ui_common.hpp"
#include "ui_sidebar.hpp"
#include "ui_chat.hpp"

namespace trc {

Rendering::Rendering(int width, int height) {
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    if (ret != 0) {
        throw NotSupportedError();
    }

    Window = UiCommon::Wrapper<SDL_Window>(
            SDL_CreateWindow("player",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             width,
                             height,
                             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
            SDL_DestroyWindow);
    AbortUnless(Window != nullptr);

    Renderer = UiCommon::Wrapper<SDL_Renderer>(
            SDL_CreateRenderer(Window.get(),
                               -1,
                               SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC |
                                       SDL_RENDERER_TARGETTEXTURE),
            SDL_DestroyRenderer);
    AbortUnless(Renderer != nullptr);

    AbortUnless(!SDL_SetRenderDrawBlendMode(Renderer.get(),
                                            SDL_BLENDMODE_BLEND));
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    HandleResize();

    StatsLastUpdate = SDL_GetTicks();
    StatsFramesSinceLastUpdate = 0;
    StatsFPS = 0.0f;
}

void Rendering::HandleResize() {
    int width;
    int height;

    SDL_GetRendererOutputSize(Renderer.get(), &width, &height);

    std::cout << "handle resize: " << width << "x" << height << std::endl;
    
    // TODO: prevent this instead of crashing
    AbortUnless(width >= 550 && height >= 450);

    if (RenderOptions.Width == width && RenderOptions.Height == height) {
        std::cerr << "bogus resize event?" << std::endl;
        return;
    }

    RenderOptions.Width = width;
    RenderOptions.Height = height;

    // Sidebar's size is always 176 x <window height>
    // Chat's height is adjustable, but its width is always <window width> - 176
    // Game's height depends on the chat's height, but its width is always <window width> - 176
    Game.UpdateSize({.x = 0, .y = 0, .w = width - 176, .h = height - 174},
                    Renderer.get());
    Sidebar.UpdateSize({.x = width - 176, .y = 0, .w = 176, .h = height},
                       Renderer.get());
    Chat.UpdateSize({.x = 0, .y = height - 174, .w = width - 176, .h = 174},
                    Renderer.get());

    // Debug
    std::cout << "Window size: " << RenderOptions.Width << "x"
              << RenderOptions.Height << std::endl;
    std::cout << "Game position: " << Game.Rect.x << ", " << Game.Rect.y
              << std::endl;
    std::cout << "Game size: " << Game.Rect.w << "x" << Game.Rect.h
              << std::endl;
    std::cout << "Gamestate position: " << Game.RectGamestate.x << ", "
              << Game.RectGamestate.y << std::endl;
    std::cout << "Gamestate size: " << Game.RectGamestate.w << "x"
              << Game.RectGamestate.h << std::endl;
    //std::cout << "Gamestate scale: " << scale << std::endl;
    std::cout << "Sidebar rect position: " << Sidebar.Rect.x << ", "
              << Sidebar.Rect.y << std::endl;
    std::cout << "Sidebar rect size: " << Sidebar.Rect.w << "x"
              << Sidebar.Rect.h << std::endl;
    std::cout << "Chat rect position: " << Chat.Rect.x << ", " << Chat.Rect.y
              << std::endl;
    std::cout << "Chat rect size: " << Chat.Rect.w << "x" << Chat.Rect.h
              << std::endl;
}

void Rendering::Render(Playback &playback) {
    Renderer::Update(RenderOptions, *playback.Gamestate);

    AbortUnless(!SDL_SetRenderDrawColor(Renderer.get(), 0, 0, 0, 255));
    AbortUnless(!SDL_RenderClear(Renderer.get()));

    Game.Render(RenderOptions, *playback.Gamestate, Renderer.get(), playback, StatsFPS);
    Sidebar.Render(RenderOptions, *playback.Gamestate, Renderer.get());
    Chat.Render(RenderOptions, *playback.Gamestate, Renderer.get());

    SDL_RenderPresent(Renderer.get());

    /* FPS counter */
    uint32_t currentTick = SDL_GetTicks();
    StatsFramesSinceLastUpdate += 1;
    if (currentTick >= StatsLastUpdate + 1000) {
        StatsFPS += StatsFramesSinceLastUpdate;
        StatsFPS /= 2;
        StatsFramesSinceLastUpdate = 0;
        StatsLastUpdate = currentTick;
    }
}

void Rendering::MouseClick(int x, int y) {
    if (UiCommon::PointIsInside(x, y, Game.Rect)) {
        Game.MouseClick(x - Game.Rect.x, y - Game.Rect.y);
    } else if (UiCommon::PointIsInside(x, y, Sidebar.Rect)) {
        Sidebar.MouseClick(x - Sidebar.Rect.x, y - Sidebar.Rect.y);
    } else if (UiCommon::PointIsInside(x, y, Chat.Rect)) {
        Chat.MouseClick(x - Chat.Rect.x, y - Chat.Rect.y);
    } else {
        AbortUnless(false);
    }
}

}; // namespace trc
