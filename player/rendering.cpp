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
                                                         int height) {
    SDL_Texture *texture = SDL_CreateTexture(SdlRenderer.get(),
                                             SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             width,
                                             height);

    AbortUnless(texture != nullptr);
    AbortUnless(!SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));

    return Rendering::Wrapper<SDL_Texture>(texture, SDL_DestroyTexture);
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

    SdlWindow = Wrapper<SDL_Window>(
            SDL_CreateWindow("player",
                             SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED,
                             width,
                             height,
                             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE),
            SDL_DestroyWindow);
    AbortUnless(SdlWindow != nullptr);

    SdlRenderer = Wrapper<SDL_Renderer>(
            SDL_CreateRenderer(SdlWindow.get(),
                               -1,
                               SDL_RENDERER_ACCELERATED |
                                       SDL_RENDERER_PRESENTVSYNC |
                                       SDL_RENDERER_TARGETTEXTURE),
            SDL_DestroyRenderer);
    AbortUnless(SdlRenderer != nullptr);

    AbortUnless(!SDL_SetRenderDrawBlendMode(SdlRenderer.get(),
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

    SDL_GetRendererOutputSize(SdlRenderer.get(), &width, &height);

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

    /* Create canvases, which are (going to be) backed by SDL_Textures */
    // TODO: Rework this. For each part of the window (gamestate, sidebar, chat)
    //       we should split the rendering into static parts and dynamic parts
    //       e.g. only re-render the inventory area if it has actually changed
    SdlTextureBackground = CreateTexture(width, height);
    BackgroundRendered = false;

    /* Gamestate canvas and texture are always the same size, so only need
     * to create them if they aren't already created */
    if (!SdlTextureGamestate) {
        CanvasGamestate = std::make_unique<Canvas>(Renderer::NativeResolutionX,
                                                   Renderer::NativeResolutionY,
                                                   Canvas::Type::External);
        SdlTextureGamestate = CreateTexture(Renderer::NativeResolutionX,
                                            Renderer::NativeResolutionY);
    }

    /* Calculate how much to scale the gamestate.
     * We want it to be centered in an area starting at the top-left of
     * the window, with exactly 176 pixels free on the right side for the sidebar
     * and 110 pixels free at the bottom for the chat.
     * We also want a 1px border and 4px margin between the gamestate and everything else */
    int margin = 4;
    int border = 1;
    int max_w = RenderOptions.Width - 176 - ((margin + border) * 2);
    int max_h = RenderOptions.Height - 110 - ((margin + border) * 2);
    float scale = std::min(max_w / (float)Renderer::NativeResolutionX,
                           max_h / (float)Renderer::NativeResolutionY);

    /* Now calculate where to render the gamestate. */
    GamestateScaledRect.x =
            ((max_w - (int)(Renderer::NativeResolutionX * scale)) / 2) + margin + border;
    GamestateScaledRect.y =
            ((max_h - (int)(Renderer::NativeResolutionY * scale)) / 2) + margin + border;
    GamestateScaledRect.w = (int)(Renderer::NativeResolutionX * scale);
    GamestateScaledRect.h = (int)(Renderer::NativeResolutionY * scale);

    CanvasOverlay = std::make_unique<Canvas>(GamestateScaledRect.w,
                                             GamestateScaledRect.h,
                                             Canvas::Type::External);
    SdlTextureOverlay = CreateTexture(GamestateScaledRect.w, GamestateScaledRect.h);

    /* We always render the sidebar on the far right side of the screen */
    SidebarRect.x = RenderOptions.Width - 176;
    SidebarRect.y = 0;
    SidebarRect.w = 176;
    SidebarRect.h = RenderOptions.Height;
    CanvasSidebar = std::make_unique<Canvas>(SidebarRect.w,
                                             SidebarRect.h,
                                             Canvas::Type::External);
    SdlTextureSidebar = CreateTexture(SidebarRect.w, SidebarRect.h);

    ChatRect.x = 0;
    ChatRect.y = RenderOptions.Height - 110;
    ChatRect.w = RenderOptions.Width - 176;
    ChatRect.h = 110;
    CanvasChat = std::make_unique<Canvas>(ChatRect.w,
                                          ChatRect.h,
                                          Canvas::Type::External);
    SdlTextureChat = CreateTexture(ChatRect.w, ChatRect.h);

    /* Debug. */
    std::cout << "Window size: " << RenderOptions.Width << "x" << RenderOptions.Height << std::endl;
    std::cout << "Gamestate scale: " << scale << std::endl;
    std::cout << "Gamestate rect position: " << GamestateScaledRect.x << ", " << GamestateScaledRect.y << std::endl;
    std::cout << "Gamestate rect size: " << GamestateScaledRect.w << "x" << GamestateScaledRect.h << std::endl;
    std::cout << "Sidebar rect position: " << SidebarRect.x << ", " << SidebarRect.y << std::endl;
    std::cout << "Sidebar rect size: " << SidebarRect.w << "x" << SidebarRect.h << std::endl;
    std::cout << "Chat rect position: " << ChatRect.x << ", " << ChatRect.y << std::endl;
    std::cout << "Chat rect size: " << ChatRect.w << "x" << ChatRect.h << std::endl;
}

void Rendering::Render(Playback &playback) {
    /* Render background (if not already done, as this only needs to be done
     * once */
    if (!BackgroundRendered) {
        Canvas background(RenderOptions.Width,
                          RenderOptions.Height,
                          Canvas::Type::External);

        AbortUnless(!SDL_LockTexture(SdlTextureBackground.get(),
                                     NULL,
                                     (void **)&background.Buffer,
                                     &background.Stride));

        // Draw background on the whole canvas
        Renderer::DrawClientBackground(*playback.Gamestate,
                                       background,
                                       0,
                                       0,
                                       background.Width,
                                       background.Height);

        // Draw gamestate background border
        Renderer::ApplyBorderHollow(background,
                                    GamestateScaledRect.x - 1,
                                    GamestateScaledRect.y - 1,
                                    GamestateScaledRect.x + GamestateScaledRect.w + 1,
                                    GamestateScaledRect.y + GamestateScaledRect.h + 1,
                                    1);

        SDL_UnlockTexture(SdlTextureBackground.get());
        BackgroundRendered = true;
    }

    /* Render gamestate */
    {
        AbortUnless(!SDL_LockTexture(SdlTextureGamestate.get(),
                                     NULL,
                                     (void **)&CanvasGamestate->Buffer,
                                     &CanvasGamestate->Stride));

        CanvasGamestate->Wipe();

        Renderer::DrawGamestate(RenderOptions,
                                *playback.Gamestate,
                                *CanvasGamestate);

        SDL_UnlockTexture(SdlTextureGamestate.get());
    }

    /* FIXME: C++ migration. */
    playback.Gamestate->Messages.Prune(playback.Gamestate->CurrentTick);

    /* Render overlay */
    {
        AbortUnless(!SDL_LockTexture(SdlTextureOverlay.get(),
                                     NULL,
                                     (void **)&CanvasOverlay->Buffer,
                                     &CanvasOverlay->Stride));

        CanvasOverlay->Wipe();

        //Renderer::DrawOverlay(RenderOptions, *playback.Gamestate, *CanvasOverlay);

        //const auto &test = playback.Gamestate->Version.Icons.Test;
        //CanvasOverlay->Draw(test, 0, 0, test.Width, test.Height);

        /* Render playback info */
        /*
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
                             *CanvasOverlay);

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
                             *CanvasOverlay);

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
                             *CanvasOverlay);

        TextRenderer::Render(playback.Gamestate->Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             56,
                             64,
                             "Playback speed: " + std::to_string(playback.Scale),
                             *CanvasOverlay);
        */
        SDL_UnlockTexture(SdlTextureOverlay.get());
    }

    /* Render sidebar */
    {
        AbortUnless(!SDL_LockTexture(SdlTextureSidebar.get(),
                                     NULL,
                                     (void **)&CanvasSidebar->Buffer,
                                     &CanvasSidebar->Stride));

        CanvasSidebar->Wipe();

        // For now, don't support changing order of these
        Renderer::DrawClientBackground(*playback.Gamestate,
                                       *CanvasSidebar,
                                       0,
                                       0,
                                       CanvasSidebar->Width,
                                       CanvasSidebar->Height);
        Renderer::ApplyBorderRaised(*CanvasSidebar, 0, 0, 176, 334, 2);
        Renderer::DrawMinimapArea(*playback.Gamestate, *CanvasSidebar);
        Renderer::DrawStatusBars(*playback.Gamestate, *CanvasSidebar);
        Renderer::DrawInventoryArea(*playback.Gamestate, *CanvasSidebar);
        /*
        offsetY += 5;

        Renderer::DrawInventoryArea(*playback.Gamestate,
                                    *CanvasSidebar,
                                    offsetX,
                                    offsetY);

        // Border around (map), status bars and inventory area
        Renderer::DrawBorderRaised(*CanvasSidebar,
                                   0,
                                   0,
                                   160,
                                   offsetY);

        if (playback.Gamestate->Version.Features.IconBar) {
            Renderer::DrawIconBar(*playback.Gamestate,
                                  *CanvasSidebar,
                                  offsetX,
                                  offsetY);
        }

        Renderer::DrawSkills(*playback.Gamestate,
                             *CanvasSidebar,
                             160 - 24,
                             offsetX,
                             offsetY);
        */

        /*
        int max_container_y = CanvasSidebar->Height - 4 - 32;

        for (auto &[_, container] : playback.Gamestate->Containers) {
            Renderer::DrawContainer(*playback.Gamestate,
                                    *CanvasSidebar,
                                    container,
                                    false,
                                    CanvasSidebar->Width,
                                    max_container_y,
                                    offsetX,
                                    offsetY);
        }
        */

        SDL_UnlockTexture(SdlTextureSidebar.get());
    }

    /* Render chat */
    {
        AbortUnless(!SDL_LockTexture(SdlTextureChat.get(),
                                     NULL,
                                     (void **)&CanvasChat->Buffer,
                                     &CanvasChat->Stride));

        CanvasChat->DrawRectangle(Pixel(0, 0, 64),
                                  0,
                                  0,
                                  CanvasChat->Width,
                                  CanvasChat->Height);

        SDL_UnlockTexture(SdlTextureChat.get());
    }

    /* Render textures to screen */
    AbortUnless(!SDL_SetRenderDrawColor(SdlRenderer.get(), 0, 0, 0, 255));
    AbortUnless(!SDL_RenderClear(SdlRenderer.get()));

    AbortUnless(!SDL_RenderCopy(SdlRenderer.get(),
                                SdlTextureBackground.get(),
                                NULL,
                                NULL));
    AbortUnless(!SDL_RenderCopy(SdlRenderer.get(),
                                SdlTextureGamestate.get(),
                                NULL,
                                &GamestateScaledRect));
    AbortUnless(!SDL_RenderCopy(SdlRenderer.get(),
                                SdlTextureOverlay.get(),
                                NULL,
                                &GamestateScaledRect));
    AbortUnless(!SDL_RenderCopy(SdlRenderer.get(),
                                SdlTextureSidebar.get(),
                                NULL,
                                &SidebarRect));
    AbortUnless(!SDL_RenderCopy(SdlRenderer.get(),
                                SdlTextureChat.get(),
                                NULL,
                                &ChatRect));
    SDL_RenderPresent(SdlRenderer.get());

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
