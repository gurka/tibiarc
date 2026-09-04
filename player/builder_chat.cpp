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

#include "builder_chat.hpp"

#include <memory>

#include <gui/border.hpp>
#include "gui/panel.hpp"
#include "gui/widget.hpp"
#include "canvas.hpp"
#include "gamestate.hpp"
#include "versions.hpp"
#include "textrenderer.hpp"

using namespace trc;

struct ChatTop : public gui::Widget {

    Gamestate *gamestate;
  
    ChatTop(int width, int height, Gamestate *gamestate)
        : Widget(width, height), gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = gamestate->Version.Icons;
        // Top border
        canvas.DrawTiled(icons.BorderHorizontalLight,
                         offset.X,
                         offset.Y,
                         offset.X + Width,
                         offset.Y + 1);
        canvas.DrawTiled(icons.BorderHorizontalDark,
                         offset.X,
                         offset.Y + 4,
                         offset.X + Width,
                         offset.Y + 5);

        // Channels background
        canvas.Draw(icons.ChatBackgroundDarkLeft, offset.X, offset.Y + 5);
        canvas.DrawTiled(icons.ChatBackgroundDark,
                         offset.X + 2,
                         offset.Y + 5,
                         offset.X + Width,
                         offset.Y + 5 + icons.ChatBackgroundDark.Height);

        // Buttons
        canvas.Draw(icons.ChatChannelButton,
                    offset.X + Width - 32,
                    offset.Y + 5);
        canvas.Draw(icons.ChatIgnoreButton,
                    offset.X + Width - 16,
                    offset.Y + 5);
    }
};

struct ChatBottom : public gui::Widget {

    Gamestate *gamestate;

    ChatBottom(int width, int height, Gamestate *gamestate)
        : Widget(width, height), gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = gamestate->Version.Icons;
        const auto &fonts = gamestate->Version.Fonts;

        // Channels
        // TODO: Order and active channel
        auto x = offset.X + 18;
        for (const auto &[id, channel] : gamestate->Channels) {
            canvas.Draw(icons.ChatChannelBoxActive, x, offset.Y + 5);
            TextRenderer::DrawCenteredString(fonts.Game,
                                             Pixel(0xDF, 0xDF, 0xDF),
                                             x + 48,
                                             offset.Y + 10,
                                             channel.Name,
                                             canvas);

            x += 100;
        }

        // Chat window
        canvas.DrawTiled(icons.ClientBackground,
                         offset.X + 2,
                         offset.Y + 23,
                         offset.X + Width - 2,
                         offset.Y + Height - 2);
        canvas.Draw(icons.ChatMessageBorderTopLeft,
                    offset.X + 4,
                    offset.Y + 26);
        canvas.DrawTiled(icons.ChatMessageBorderHorizontal,
                         offset.X + 7,
                         offset.Y + 26,
                         offset.X + Width - 7,
                         offset.Y + 29);
        canvas.Draw(icons.ChatMessageBorderTopRight, offset.X + Width - 7, offset.Y + 26);
        canvas.DrawTiled(icons.ChatMessageBorderVertical,
                               offset.X + 4,
                               offset.Y + 29,
                               offset.X + 7,
                               offset.Y + Height - 25);
        canvas.DrawTiled(icons.ChatMessageBorderVertical,
                               offset.X + Width - 7,
                               offset.Y + 29,
                               offset.X + Width - 4,
                               offset.Y + Height - 25);
        canvas.Draw(icons.ChatMessageBorderBottomLeft, offset.X + 4, offset.Y + Height - 25);
        canvas.DrawTiled(icons.ChatMessageBorderHorizontal,
                               offset.X + 7,
                               offset.Y + Height - 25,
                               offset.X + Width - 7,
                               offset.Y + Height - 22);
        canvas.Draw(icons.ChatMessageBorderBottomRight,
                     offset.X + Width - 7,
                     offset.Y + Height - 25);
    }
};

void Builder::ChatPanel::SetLayoutSize(int width, int height) {
    Panel::SetLayoutSize(width, height);
    Top->SetLayoutSize(width, 21);
    Bottom->GetChild().SetLayoutSize(width - 4, height - 21 - 4);
}

std::unique_ptr<Builder::ChatPanel> Builder::BuildChat(int width,
                                                       int height,
                                                       Gamestate *gamestate) {
    // Chat consists of:
    // - ChatPanel, Panel (root)
    //   - ChatTop, Widget (top part with channels and buttons)
    //   - Border
    //     - ChatBottom, Widget (bottom part with chat and input box)
    auto chatTop = std::make_unique<ChatTop>(width, 21, gamestate);
    auto chatBottom = std::make_unique<gui::Border>(
            &gamestate->Version.Icons,
            gui::Border::BorderType::Raised,
            std::move(
                    std::make_unique<ChatBottom>(width - 4, height - 21 - 4, gamestate)));

    auto panel = std::make_unique<Builder::ChatPanel>(width,
                                                      height,
                                                      std::move(chatTop),
                                                      std::move(chatBottom));
    panel->SetBackground(&gamestate->Version.Icons.ClientBackground);

    return panel;
}
