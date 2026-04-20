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

#include "ui_game.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>

#include "versions.hpp"
#include "textrenderer.hpp"

namespace {
int formatTime(char *buffer, size_t bufferSize, uint64_t milliseconds) {
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
} // namespace

namespace trc {

void UiGame::UpdateSize(SDL_Rect rect, SDL_Renderer *renderer) {
    Rect = rect;
    Canvas = std::make_unique<trc::Canvas>(Rect.w,
                                           Rect.h,
                                           trc::Canvas::Type::External);
    Texture = UiCommon::CreateTexture(renderer, Rect.w, Rect.h);

    // Gamestate canvas and texture are always the same size, so only need
    // to create them if they aren't already created
    if (!TextureGamestate) {
        CanvasGamestate =
                std::make_unique<trc::Canvas>(Renderer::NativeResolutionX,
                                              Renderer::NativeResolutionY,
                                              Canvas::Type::External);
        TextureGamestate = UiCommon::CreateTexture(renderer,
                                                   Renderer::NativeResolutionX,
                                                   Renderer::NativeResolutionY);
    }

    // Calculate how much to scale the gamestate
    int margin = 4;
    int border = 1;
    int max_w = Rect.w - ((margin + border) * 2);
    int max_h = Rect.h - ((margin + border) * 2);
    double scale = std::min(max_w / (double)Renderer::NativeResolutionX,
                            max_h / (double)Renderer::NativeResolutionY);

    // Calculate where to render the gamestate
    RectGamestate.x =
            ((max_w - (int)(Renderer::NativeResolutionX * scale)) / 2) +
            margin + border;
    RectGamestate.y =
            ((max_h - (int)(Renderer::NativeResolutionY * scale)) / 2) +
            margin + border;
    RectGamestate.w = (int)(Renderer::NativeResolutionX * scale);
    RectGamestate.h = (int)(Renderer::NativeResolutionY * scale);

    CanvasOverlay = std::make_unique<trc::Canvas>(RectGamestate.w,
                                                  RectGamestate.h,
                                                  Canvas::Type::External);
    TextureOverlay =
            UiCommon::CreateTexture(renderer, RectGamestate.w, RectGamestate.h);
}

void UiGame::Render(const Renderer::Options &renderOptions,
                    const Gamestate &gamestate,
                    SDL_Renderer *renderer,
                    const Playback &playback,
                    double statsFPS) const {
    // Render base
    {
        AbortUnless(!SDL_LockTexture(Texture.get(),
                                     NULL,
                                     (void **)&Canvas->Buffer,
                                     &Canvas->Stride));

        Canvas->Wipe();

        // Draw background
        Canvas->DrawBackground(gamestate.Version.Icons.ClientBackground,
                               0,
                               0,
                               Canvas->Width,
                               Canvas->Height);

        // Clear the area where the gamestate will be rendered
        Canvas->DrawRectangle(Pixel(0, 0, 0, 0),
                              RectGamestate.x,
                              RectGamestate.y,
                              RectGamestate.w,
                              RectGamestate.h);

        // Draw border around the gamestate
        UiCommon::DrawBorder1px(gamestate.Version.Icons,
                                *Canvas,
                                RectGamestate.x - 1,
                                RectGamestate.y - 1,
                                RectGamestate.x + RectGamestate.w + 1,
                                RectGamestate.y + RectGamestate.h + 1);

        // Render playback info
        char text[64];
        int textLength = snprintf(text,
                                  sizeof(text) / sizeof(text[0]),
                                  "FPS: %.2f",
                                  statsFPS);
        TextRenderer::Render(gamestate.Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             14,
                             64,
                             std::string(text, textLength),
                             *Canvas);

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
                             *Canvas);

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
                             *Canvas);

        TextRenderer::Render(playback.Gamestate->Version.Fonts.Game,
                             TextAlignment::Left,
                             TextTransform::None,
                             Pixel(0xFF, 0xFF, 0xFF),
                             12,
                             56,
                             64,
                             "Playback speed: " +
                                     std::to_string(playback.Scale),
                             *Canvas);

        SDL_UnlockTexture(Texture.get());
    }

    // Render gamestate
    {
        AbortUnless(!SDL_LockTexture(TextureGamestate.get(),
                                     NULL,
                                     (void **)&CanvasGamestate->Buffer,
                                     &CanvasGamestate->Stride));

        CanvasGamestate->Wipe();
        Renderer::DrawGamestate(renderOptions, gamestate, *CanvasGamestate);

        SDL_UnlockTexture(TextureGamestate.get());
    }

    // Render overlay
    {
        AbortUnless(!SDL_LockTexture(TextureOverlay.get(),
                                     NULL,
                                     (void **)&CanvasOverlay->Buffer,
                                     &CanvasOverlay->Stride));

        CanvasOverlay->Wipe();
        Renderer::DrawOverlay(renderOptions, gamestate, *CanvasOverlay);

        SDL_UnlockTexture(TextureOverlay.get());
    }

    // Render textures to window
    AbortUnless(!SDL_RenderCopy(renderer,
                                TextureGamestate.get(),
                                NULL,
                                &RectGamestate));
    AbortUnless(!SDL_RenderCopy(renderer,
                                TextureOverlay.get(),
                                NULL,
                                &RectGamestate));
    AbortUnless(!SDL_RenderCopy(renderer,
                                Texture.get(),
                                NULL,
                                &Rect));
}

void UiGame::MouseClick(int x, int y) {
    std::cout << "UiGame mouse click: " << x << ", " << y << "\n";
}

} // namespace trc
