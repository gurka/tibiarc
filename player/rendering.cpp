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

#include "rendering.hpp"

#include <iostream>

#include "canvas.hpp"
#include "versions.hpp"
#include "textrenderer.hpp"
#include "ui_renderer.hpp"

namespace trc {

Rendering::Wrapper<SDL_Texture> Rendering::CreateTexture(int width,
                                                         int height) const {
    SDL_Texture *texture = SDL_CreateTexture(Renderer.get(),
                                             SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             width,
                                             height);

    AbortUnless(texture != nullptr);
    AbortUnless(!SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));

    return Rendering::Wrapper<SDL_Texture>(texture, SDL_DestroyTexture);
}

void Rendering::ResetArea(Area &area, int x, int y, int w, int h) const {
    area.Rect.x = x;
    area.Rect.y = y;
    area.Rect.w = w;
    area.Rect.h = h;
    area.Canvas = std::make_unique<Canvas>(w, h, trc::Canvas::Type::External);
    area.Texture = CreateTexture(w, h);
}

static int formatTime(char *buffer, size_t bufferSize, uint64_t milliseconds) {
    /* TODO: move to utils.hpp or similar? */
    uint64_t seconds = milliseconds / 1000;
    milliseconds %= 1000;
    uint64_t minutes = seconds / 60;
    seconds %= 60;
    uint64_t hours = minutes / 60;
    minutes %= 60;

    return snprintf(buffer,
                    bufferSize,
                    "%02llu:%02llu:%02llu.%03llu",
                    hours,
                    minutes,
                    seconds,
                    milliseconds);
}

Rendering::Rendering(int width, int height) {
    int ret = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    if (ret != 0) {
        throw NotSupportedError();
    }

    Window = Wrapper<SDL_Window>(
            SDL_CreateWindow("player",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             width,
                             height,
                             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
            SDL_DestroyWindow);
    AbortUnless(Window != nullptr);

    Renderer = Wrapper<SDL_Renderer>(
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

    /* Setup rendering */
    RenderOptions.Width = width;
    RenderOptions.Height = height;

    // Setup all areas
    // For now the chat is not resizable and always have a height of 174
    ResetArea(Game, 0, 0, width - 176, height - 174);
    ResetArea(Sidebar, width - 176, 0, 176, height);
    ResetArea(Chat, 0, height - 174, width - 176, 174);

    /* Gamestate canvas and texture are always the same size, so only need
     * to create them if they aren't already created */
    if (!TextureGamestate) {
        CanvasGamestate = std::make_unique<Canvas>(Renderer::NativeResolutionX,
                                                   Renderer::NativeResolutionY,
                                                   Canvas::Type::External);
        TextureGamestate = CreateTexture(Renderer::NativeResolutionX,
                                         Renderer::NativeResolutionY);
    }

    // Calculate how much to scale the gamestate
    int margin = 4;
    int border = 1;
    int max_w = Game.Rect.w - ((margin + border) * 2);
    int max_h = Game.Rect.h - ((margin + border) * 2);
    double scale = std::min(max_w / (double)Renderer::NativeResolutionX,
                            max_h / (double)Renderer::NativeResolutionY);

    /* Now calculate where to render the gamestate. */
    RectGamestate.x =
            ((max_w - (int)(Renderer::NativeResolutionX * scale)) / 2) +
            margin + border;
    RectGamestate.y =
            ((max_h - (int)(Renderer::NativeResolutionY * scale)) / 2) +
            margin + border;
    RectGamestate.w = (int)(Renderer::NativeResolutionX * scale);
    RectGamestate.h = (int)(Renderer::NativeResolutionY * scale);

    // Overlay canvas and texture
    CanvasOverlay = std::make_unique<Canvas>(RectGamestate.w,
                                             RectGamestate.h,
                                             Canvas::Type::External);
    TextureOverlay = CreateTexture(RectGamestate.w, RectGamestate.h);

    /* Debug. */
    std::cout << "Window size: " << RenderOptions.Width << "x"
              << RenderOptions.Height << std::endl;
    std::cout << "Game position: " << Game.Rect.x << ", " << Game.Rect.y
              << std::endl;
    std::cout << "Game size: " << Game.Rect.w << "x" << Game.Rect.h
              << std::endl;
    std::cout << "Gamestate position: " << RectGamestate.x << ", "
              << RectGamestate.y << std::endl;
    std::cout << "Gamestate size: " << RectGamestate.w << "x" << RectGamestate.h
              << std::endl;
    std::cout << "Gamestate scale: " << scale << std::endl;
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
    /* FIXME: C++ migration. */
    playback.Gamestate->Messages.Prune(playback.Gamestate->CurrentTick);

    // Render gamestate
    {
        AbortUnless(!SDL_LockTexture(TextureGamestate.get(),
                                     NULL,
                                     (void **)&CanvasGamestate->Buffer,
                                     &CanvasGamestate->Stride));

        CanvasGamestate->Wipe();
        Renderer::DrawGamestate(RenderOptions,
                                *playback.Gamestate,
                                *CanvasGamestate);

        SDL_UnlockTexture(TextureGamestate.get());
    }

    // Render overlay
    {
        AbortUnless(!SDL_LockTexture(TextureOverlay.get(),
                                     NULL,
                                     (void **)&CanvasOverlay->Buffer,
                                     &CanvasOverlay->Stride));

        CanvasOverlay->Wipe();
        Renderer::DrawOverlay(RenderOptions, *playback.Gamestate, *CanvasOverlay);

        SDL_UnlockTexture(TextureOverlay.get());
    }

    // Render game area
    {
        AbortUnless(!SDL_LockTexture(Game.Texture.get(),
                                     NULL,
                                     (void **)&Game.Canvas->Buffer,
                                     &Game.Canvas->Stride));

        Game.Canvas->Wipe();

        // Draw background
        UiRenderer::DrawBackground(
                playback.Gamestate->Version.Icons.ClientBackground,
                *Game.Canvas,
                0,
                0,
                Game.Canvas->Width,
                Game.Canvas->Height);

        // Draw border around the gamestate
        UiRenderer::DrawBorder1px(playback.Gamestate->Version.Icons,
                                  *Game.Canvas,
                                  RectGamestate.x - 1,
                                  RectGamestate.y - 1,
                                  RectGamestate.x + RectGamestate.w + 1,
                                  RectGamestate.y + RectGamestate.h + 1);

        // Render playback info
        char text[64];
        int textLength = snprintf(text,
                                  sizeof(text) / sizeof(text[0]),
                                  "FPS: %.2f",
                                  StatsFPS);
        TextRenderer::Render(playback.Gamestate->Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             14,
                             64,
                             std::string(text, textLength),
                             *Game.Canvas);

        textLength = formatTime(text,
                                sizeof(text) / sizeof(text[0]),
                                playback.GetPlaybackTick());
        TextRenderer::Render(playback.Gamestate->Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             28,
                             64,
                             std::string(text, textLength),
                             *Game.Canvas);

        textLength = formatTime(text,
                                sizeof(text) / sizeof(text[0]),
                                playback.Recording->Runtime.count());
        TextRenderer::Render(playback.Gamestate->Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             42,
                             64,
                             std::string(text, textLength),
                             *Game.Canvas);

        TextRenderer::Render(playback.Gamestate->Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             56,
                             64,
                             "Playback speed: " + std::to_string(playback.Scale),
                             *Game.Canvas);

        SDL_UnlockTexture(Game.Texture.get());
    }


    // Render sidebar area
    {
        AbortUnless(!SDL_LockTexture(Sidebar.Texture.get(),
                                     NULL,
                                     (void **)&Sidebar.Canvas->Buffer,
                                     &Sidebar.Canvas->Stride));

        Sidebar.Canvas->Wipe();

        UiRenderer::DrawSidebarTop(*playback.Gamestate,
                                 *Sidebar.Canvas);

        int offsetY = UiRenderer::DrawSidebarTop(*playback.Gamestate,
                                               *Sidebar.Canvas);
        UiRenderer::DrawSidebarMiddle(*playback.Gamestate,
                                    *Sidebar.Canvas,
                                    offsetY);
        UiRenderer::DrawSidebarBottom(*playback.Gamestate,
                                    *Sidebar.Canvas,
                                    offsetY);

        SDL_UnlockTexture(Sidebar.Texture.get());
    }

    // Render chat area
    {
        AbortUnless(!SDL_LockTexture(Chat.Texture.get(),
                                     NULL,
                                     (void **)&Chat.Canvas->Buffer,
                                     &Chat.Canvas->Stride));

        Chat.Canvas->Wipe();
        UiRenderer::DrawChat(*playback.Gamestate, *Chat.Canvas);

        SDL_UnlockTexture(Chat.Texture.get());
    }

    /* Render textures to screen */
    AbortUnless(!SDL_SetRenderDrawColor(Renderer.get(), 0, 0, 0, 255));
    AbortUnless(!SDL_RenderClear(Renderer.get()));

    // Game
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Game.Texture.get(),
                                NULL,
                                &Game.Rect));
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                TextureGamestate.get(),
                                NULL,
                                &RectGamestate));
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                TextureOverlay.get(),
                                NULL,
                                &RectGamestate));

    // Sidebar
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Sidebar.Texture.get(),
                                NULL,
                                &Sidebar.Rect));

    // Chat
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Chat.Texture.get(),
                                NULL,
                                &Chat.Rect));

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

}; // namespace trc
