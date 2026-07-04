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

#include "builder.hpp"

#include <memory>

#include "builder_sidebar.hpp"
#include "builder_game.hpp"
#include "state.hpp"

#include "gui/panel.hpp"
#include "gui/position.hpp"
#include "gamestate.hpp"
#include "renderer.hpp"

using namespace trc;

std::unique_ptr<Builder::Gui> Builder::BuildGui(int windowWidth,
                                                int windowHeight,
                                                trc::Gamestate *gamestate,
                                                GuiState *guiState) {
    // Calculate size of gui widgets
    auto sidebarWidth = 176; // always 176
    auto sidebarHeight = windowHeight;
    auto chatWidth = windowWidth - sidebarWidth;
    auto chatHeight = 174; // always 174, for now
    auto gameWidth = windowWidth - sidebarWidth;
    auto gameHeight = windowHeight - chatHeight;

    // Calculate size of gamestate
    auto margin = 4;
    auto border = 1;
    auto maxWidth = gameWidth - ((margin + border) * 2);
    auto maxHeight = gameHeight - ((margin + border) * 2);
    auto scale = std::min(
            maxWidth / static_cast<double>(Renderer::NativeResolutionX),
            maxHeight / static_cast<double>(Renderer::NativeResolutionY));

    auto gamestateX = ((maxWidth -
                        static_cast<int>(Renderer::NativeResolutionX * scale)) /
                       2) +
                      margin + border;
    auto gamestateY = ((maxHeight -
                        static_cast<int>(Renderer::NativeResolutionY * scale)) /
                       2) +
                      margin + border;
    auto gamestateWidth = static_cast<int>(Renderer::NativeResolutionX * scale);
    auto gamestateHeight =
            static_cast<int>(Renderer::NativeResolutionY * scale);

    // Build gui
    auto panel = std::make_unique<gui::Panel>(windowWidth, windowHeight);
    panel->Add(Builder::BuildSidebar(windowHeight, gamestate, guiState),
               gui::Position(windowWidth - 176, 0));
    /*
    panel->Add(Builder::BuildChat(windowWidth - 176,
                                174,
                                gamestate,
                                guiState),
             gui::Position(0, windowHeight - 174));
    */
    panel->Add(Builder::BuildGame(windowWidth - 176,
                                  windowHeight - 174,
                                  gamestate,
                                  guiState,
                                  gamestateX,
                                  gamestateY,
                                  gamestateWidth,
                                  gamestateHeight),
               gui::Position(0, 0));

    return std::make_unique<Builder::Gui>(Builder::Gui{std::move(panel),
                                                       gamestateX,
                                                       gamestateY,
                                                       gamestateWidth,
                                                       gamestateHeight});

}
