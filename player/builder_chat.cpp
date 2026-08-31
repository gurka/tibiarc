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

    ChatBottom(int width, int height) : Widget(width, height) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
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
                    std::make_unique<ChatBottom>(width - 4, height - 21 - 4)));

    auto panel = std::make_unique<Builder::ChatPanel>(width,
                                                      height,
                                                      std::move(chatTop),
                                                      std::move(chatBottom));
    panel->SetBackground(&gamestate->Version.Icons.ClientBackground);

    return panel;
}
