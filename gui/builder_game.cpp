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

#include "builder_game.hpp"

#include <algorithm>
#include <memory>

#include "state.hpp"

#include "gui/border.hpp"
#include "gui/panel.hpp"
#include "gui/widget.hpp"
#include "canvas.hpp"
#include "gamestate.hpp"
#include "renderer.hpp"
#include "versions.hpp"

using namespace trc;

// This is just a dummy widget
// Rendering the game happens in gui_test.cpp
struct GamestateWidget : public gui::Widget {
    GamestateWidget(int width, int height) : Widget(width, height) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
    }
};

void Builder::GamePanel::SetGamestateBounds(int x,
                                          int y,
                                          int width,
                                          int height) {
    if (GamestateWidget != nullptr) {
        GamestateWidget->SetLayoutSize(width, height);
    }
    if (GamestateBorder != nullptr) {
        SetChildPosition(GamestateBorder, gui::Position(x - 1, y - 1));
    }
}

std::unique_ptr<Builder::GamePanel> Builder::BuildGame(int width,
                                                       int height,
                                                       trc::Gamestate *gamestate,
                                                       GuiState *guiState,
                                                       int gamestateX,
                                                       int gamestateY,
                                                       int gamestateWidth,
                                                       int gamestateHeight) {

    auto panel = std::make_unique<Builder::GamePanel>(width, height);
    panel->SetBackground(&gamestate->Version.Icons.ClientBackground);

    auto gameWidget = std::make_unique<GamestateWidget>(gamestateWidth, gamestateHeight);
    panel->GamestateWidget = gameWidget.get();

    auto borderWidget =
            std::make_unique<gui::Border>(&gamestate->Version.Icons,
                                          gui::Border::BorderType::Sunken,
                                          std::move(gameWidget));
    panel->GamestateBorder = borderWidget.get();

    panel->Add(std::move(borderWidget),
               gui::Position(gamestateX - 1, gamestateY - 1));

    return panel;
}
