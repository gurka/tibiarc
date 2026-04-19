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

#include "ui_common.hpp"
#include "versions.hpp"
#include "textrenderer.hpp"
#include "renderer.hpp"
#include "canvas.hpp"
#include "icons.hpp"
#include "fonts.hpp"
#include "pixel.hpp"

namespace trc {
namespace UiChat {

void DrawChat(Gamestate &gamestate, Canvas &canvas) noexcept {
    const auto &icons = gamestate.Version.Icons;

    // Top border
    canvas.DrawBackground(icons.BorderHorizontalLight, 0, 0, canvas.Width, 1);
    canvas.DrawBackground(icons.ClientBackground, 0, 1, canvas.Width, 4);
    canvas.DrawBackground(icons.BorderHorizontalDark, 0, 4, canvas.Width, 5);

    // Window names background
    canvas.Draw(icons.ChatBackgroundDarkLeft, 0, 5);
    canvas.DrawBackground(icons.ChatBackgroundDark, 2, 5, canvas.Width, 21);

    // Buttons
    canvas.Draw(icons.ChatChannelButton, canvas.Width - 32, 5);
    canvas.Draw(icons.ChatIgnoreButton, canvas.Width - 16, 5);

    // Message window border
    UiCommon::DrawBorder2px(icons, canvas, 0, 21, canvas.Width, canvas.Height);

    canvas.Draw(icons.ChatChannelBoxActive, 18, 5);

    // TODO: spacing is 0 but it still too much, maybe we need a Chat font with -1 spacing?
    //       verify if this is for all text in chat or only chat window title
    TextRenderer::DrawCenteredString(gamestate.Version.Fonts.Game,
                                     Pixel(0xDF, 0xDF, 0xDF),
                                     66,
                                     10,
                                     "Default",
                                     canvas);

    // Message window background
    canvas.DrawBackground(icons.ClientBackground,
                          2,
                          23,
                          canvas.Width - 2,
                          canvas.Height - 2);

    // Message border
    canvas.Draw(icons.ChatMessageBorderTopLeft, 4, 26);
    canvas.DrawBackground(icons.ChatMessageBorderHorizontal,
                          7,
                          26,
                          canvas.Width - 7,
                          29);
    canvas.Draw(icons.ChatMessageBorderTopRight, canvas.Width - 7, 26);
    canvas.DrawBackground(icons.ChatMessageBorderVertical,
                          4,
                          29,
                          7,
                          canvas.Height - 25);
    canvas.DrawBackground(icons.ChatMessageBorderVertical,
                          canvas.Width - 7,
                          29,
                          canvas.Width - 4,
                          canvas.Height - 25);
    canvas.Draw(icons.ChatMessageBorderBottomLeft, 4, canvas.Height - 25);
    canvas.DrawBackground(icons.ChatMessageBorderHorizontal,
                          7,
                          canvas.Height - 25,
                          canvas.Width - 7,
                          canvas.Height - 22);
    canvas.Draw(icons.ChatMessageBorderBottomRight, canvas.Width - 7, canvas.Height - 25);

    // Scrollbar
    canvas.Draw(icons.ScrollbarUp, canvas.Width - 19, 29);
    canvas.DrawBackground(icons.ScrollbarBackground,
                          canvas.Width - 19,
                          29 + 12,
                          canvas.Width - 19 + 12,
                          canvas.Height - 37);
    canvas.Draw(icons.ScrollbarDown, canvas.Width - 19, canvas.Height - 37);

    // Message input box
    canvas.Draw(icons.ChatTalkButton, 5, canvas.Height - 20);
    UiCommon::DrawBorder1px(icons,
                            canvas,
                            23,
                            canvas.Height - 20,
                            canvas.Width - 4,
                            canvas.Height - 4);
    canvas.DrawRectangle(Pixel(0x36, 0x36, 0x36),
                         24,
                         canvas.Height - 19,
                         canvas.Width - 29,
                         14);
}

} // namespace UiChat
} // namespace trc
