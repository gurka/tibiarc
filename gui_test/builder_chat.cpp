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

#include "state.hpp"

#include "gui/panel.hpp"
#include "gui/widget.hpp"
#include "canvas.hpp"
#include "gamestate.hpp"
#include "versions.hpp"

using namespace trc;

// Placeholder widget for chat area
struct ChatPlaceholder : public gui::Widget {
    ChatPlaceholder(int width, int height) : Widget(width, height) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        // TODO: Implement chat rendering
    }
};

std::unique_ptr<gui::Widget> Builder::BuildChat(int width,
                                                int height,
                                                trc::Gamestate *gamestate,
                                                GuiState *guiState) {

    auto panel = std::make_unique<gui::Panel>(width, height);
    panel->SetBackground(&gamestate->Version.Icons.ClientBackground);
    auto chatWidget = std::make_unique<ChatPlaceholder>(width, height);
    panel->Add(std::move(chatWidget), gui::Position(0, 0));

    return panel;
}
