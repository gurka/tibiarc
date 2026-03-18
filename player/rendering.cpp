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

#include "versions.hpp"
#include "textrenderer.hpp"

#include <iostream>

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
    area.CanvasStatic = std::make_unique<Canvas>(w, h, trc::Canvas::Type::External);
    area.TextureStatic = CreateTexture(w, h);
    area.CanvasDynamic = std::make_unique<Canvas>(w, h, trc::Canvas::Type::External);
    area.TextureDynamic = CreateTexture(w, h);
    area.StaticRendered = false;
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
    if (width < 666 || height < 472) {
        // TODO: prevent this instead of crashing
        abort();
    }
    if (RenderOptions.Width == width && RenderOptions.Height == height) {
        std::cerr << "bogus resize event?" << std::endl;
        return;
    }

    /* Setup rendering */
    RenderOptions.Width = width;
    RenderOptions.Height = height;

    // Setup all areas
    // For now the chat is not resizable and always have a height of 110
    ResetArea(Game, 0, 0, width - 176, height - 110);
    ResetArea(Sidebar, width - 176, 0, 176, height);
    ResetArea(Chat, 0, height - 110, width - 176, 110);

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
    float scale = std::min(max_w / (float)Renderer::NativeResolutionX,
                           max_h / (float)Renderer::NativeResolutionY);

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
    // Render game
    {
        if (!Game.StaticRendered) {
            AbortUnless(!SDL_LockTexture(Game.TextureStatic.get(),
                                         NULL,
                                         (void **)&Game.CanvasStatic->Buffer,
                                         &Game.CanvasStatic->Stride));

            // Draw background
            Renderer::DrawBackground(
                    playback.Gamestate->Version.Icons.ClientBackground,
                    *Game.CanvasStatic,
                    0,
                    0,
                    Game.CanvasStatic->Width,
                    Game.CanvasStatic->Height);

            // Draw border around the gamestate
            // TODO

            SDL_UnlockTexture(Game.TextureStatic.get());

            Game.StaticRendered = true;
        }

        AbortUnless(!SDL_LockTexture(Game.TextureDynamic.get(),
                                     NULL,
                                     (void **)&Game.CanvasDynamic->Buffer,
                                     &Game.CanvasDynamic->Stride));

        Game.CanvasDynamic->Wipe();


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
                             *Game.CanvasDynamic);

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
                             *Game.CanvasDynamic);

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
                             *Game.CanvasDynamic);

        TextRenderer::Render(playback.Gamestate->Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             56,
                             64,
                             "Playback speed: " + std::to_string(playback.Scale),
                             *Game.CanvasDynamic);

        SDL_UnlockTexture(Game.TextureDynamic.get());
    }


    // Render sidebar
    {
        if (!Sidebar.StaticRendered) {
            // TODO: We can render SidebarTop's background and border here
            Sidebar.StaticRendered = true;
        }

        AbortUnless(!SDL_LockTexture(Sidebar.TextureDynamic.get(),
                                     NULL,
                                     (void **)&Sidebar.CanvasDynamic->Buffer,
                                     &Sidebar.CanvasDynamic->Stride));

        Sidebar.CanvasDynamic->DrawRectangle(Pixel(0, 0, 0),
                                             0,
                                             0,
                                             Sidebar.CanvasDynamic->Width,
                                             Sidebar.CanvasDynamic->Height);
        int offsetY = Renderer::DrawSidebarTop(*playback.Gamestate,
                                               *Sidebar.CanvasDynamic);
        Renderer::DrawSidebarMiddle(*playback.Gamestate,
                                    *Sidebar.CanvasDynamic,
                                    offsetY);
        Renderer::DrawSidebarBottom(*playback.Gamestate,
                                    *Sidebar.CanvasDynamic,
                                    offsetY);

        SDL_UnlockTexture(Sidebar.TextureDynamic.get());
    }

    // Render chat
    {
        if (!Chat.StaticRendered) {
            AbortUnless(!SDL_LockTexture(Chat.TextureStatic.get(),
                                         NULL,
                                         (void **)&Chat.CanvasStatic->Buffer,
                                         &Chat.CanvasStatic->Stride));

            Chat.CanvasStatic->DrawRectangle(Pixel(0, 0, 64),
                                             0,
                                             0,
                                             Chat.CanvasStatic->Width,
                                             Chat.CanvasStatic->Height);

            SDL_UnlockTexture(Chat.TextureStatic.get());

            Chat.StaticRendered = true;
        }

        AbortUnless(!SDL_LockTexture(Chat.TextureDynamic.get(),
                                     NULL,
                                     (void **)&Chat.CanvasDynamic->Buffer,
                                     &Chat.CanvasDynamic->Stride));

        Chat.CanvasDynamic->Wipe();

        SDL_UnlockTexture(Chat.TextureDynamic.get());
    }

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

    /* FIXME: C++ migration. */
    playback.Gamestate->Messages.Prune(playback.Gamestate->CurrentTick);

    /* Render textures to screen */
    AbortUnless(!SDL_SetRenderDrawColor(Renderer.get(), 0, 0, 0, 255));
    AbortUnless(!SDL_RenderClear(Renderer.get()));

    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Game.TextureStatic.get(),
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
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Game.TextureDynamic.get(),
                                NULL,
                                &Game.Rect));

    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Sidebar.TextureStatic.get(),
                                NULL,
                                &Sidebar.Rect));
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Sidebar.TextureDynamic.get(),
                                NULL,
                                &Sidebar.Rect));

    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Chat.TextureStatic.get(),
                                NULL,
                                &Chat.Rect));
    AbortUnless(!SDL_RenderCopy(Renderer.get(),
                                Chat.TextureDynamic.get(),
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
