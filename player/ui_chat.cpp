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

#include "ui_chat.hpp"

#include <iostream>

#include "ui_common.hpp"
#include "versions.hpp"
#include "textrenderer.hpp"
#include "renderer.hpp"
#include "canvas.hpp"
#include "icons.hpp"
#include "fonts.hpp"
#include "pixel.hpp"

namespace trc {

void UiChat::UpdateSize(SDL_Rect rect, SDL_Renderer *renderer) {
    Rect = rect;
    Canvas = std::make_unique<trc::Canvas>(Rect.w,
                                           Rect.h,
                                           trc::Canvas::Type::External);
    Texture = UiCommon::CreateTexture(renderer, Rect.w, Rect.h);
}

void UiChat::Render(const Renderer::Options &renderOptions,
                    const Gamestate &gamestate,
                    SDL_Renderer *renderer) const {
    // Render on canvas to texture
    AbortUnless(!SDL_LockTexture(Texture.get(),
                                 NULL,
                                 (void **)&Canvas->Buffer,
                                 &Canvas->Stride));

    Canvas->Wipe();

    const auto &icons = gamestate.Version.Icons;

    // Top border
    Canvas->DrawBackground(icons.BorderHorizontalLight, 0, 0, Canvas->Width, 1);
    Canvas->DrawBackground(icons.ClientBackground, 0, 1, Canvas->Width, 4);
    Canvas->DrawBackground(icons.BorderHorizontalDark, 0, 4, Canvas->Width, 5);

    // Window names background
    Canvas->Draw(icons.ChatBackgroundDarkLeft, 0, 5);
    Canvas->DrawBackground(icons.ChatBackgroundDark, 2, 5, Canvas->Width, 21);

    // Buttons
    Canvas->Draw(icons.ChatChannelButton, Canvas->Width - 32, 5);
    Canvas->Draw(icons.ChatIgnoreButton, Canvas->Width - 16, 5);

    // Message window border
    UiCommon::DrawBorder2px(icons,
                            *Canvas,
                            0,
                            21,
                            Canvas->Width,
                            Canvas->Height);

    Canvas->Draw(icons.ChatChannelBoxActive, 18, 5);

    // TODO: spacing is 0 but it still too much, maybe we need a Chat font with
    // -1 spacing?
    //       verify if this is for all text in chat or only chat window title
    TextRenderer::DrawCenteredString(gamestate.Version.Fonts.Game,
                                     Pixel(0xDF, 0xDF, 0xDF),
                                     66,
                                     10,
                                     "Default",
                                     *Canvas);

    // Message window background
    Canvas->DrawBackground(icons.ClientBackground,
                           2,
                           23,
                           Canvas->Width - 2,
                           Canvas->Height - 2);

    // Message border
    Canvas->Draw(icons.ChatMessageBorderTopLeft, 4, 26);
    Canvas->DrawBackground(icons.ChatMessageBorderHorizontal,
                           7,
                           26,
                           Canvas->Width - 7,
                           29);
    Canvas->Draw(icons.ChatMessageBorderTopRight, Canvas->Width - 7, 26);
    Canvas->DrawBackground(icons.ChatMessageBorderVertical,
                           4,
                           29,
                           7,
                           Canvas->Height - 25);
    Canvas->DrawBackground(icons.ChatMessageBorderVertical,
                           Canvas->Width - 7,
                           29,
                           Canvas->Width - 4,
                           Canvas->Height - 25);
    Canvas->Draw(icons.ChatMessageBorderBottomLeft, 4, Canvas->Height - 25);
    Canvas->DrawBackground(icons.ChatMessageBorderHorizontal,
                           7,
                           Canvas->Height - 25,
                           Canvas->Width - 7,
                           Canvas->Height - 22);
    Canvas->Draw(icons.ChatMessageBorderBottomRight,
                 Canvas->Width - 7,
                 Canvas->Height - 25);

    // Scrollbar
    Canvas->Draw(icons.ScrollbarUp, Canvas->Width - 19, 29);
    Canvas->DrawBackground(icons.ScrollbarBackground,
                           Canvas->Width - 19,
                           29 + 12,
                           Canvas->Width - 19 + 12,
                           Canvas->Height - 37);
    Canvas->Draw(icons.ScrollbarDown, Canvas->Width - 19, Canvas->Height - 37);

    // Message input box
    Canvas->Draw(icons.ChatTalkButton, 5, Canvas->Height - 20);
    UiCommon::DrawBorder1px(icons,
                            *Canvas,
                            23,
                            Canvas->Height - 20,
                            Canvas->Width - 4,
                            Canvas->Height - 4);
    Canvas->DrawRectangle(Pixel(0x36, 0x36, 0x36),
                          24,
                          Canvas->Height - 19,
                          Canvas->Width - 29,
                          14);

    SDL_UnlockTexture(Texture.get());

    // Render texture to window
    AbortUnless(!SDL_RenderCopy(renderer,
                                Texture.get(),
                                NULL,
                                &Rect));
}

void UiChat::MouseClick(int x, int y) {
    std::cout << "UiChat mouse click: " << x << ", " << y << "\n";
}

} // namespace trc
