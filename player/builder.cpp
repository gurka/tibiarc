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

#include <algorithm>
#include <memory>

#include "builder_sidebar.hpp"
#include "builder_game.hpp"
#include "builder_chat.hpp"
#include "player_state.hpp"

#include "gui/panel.hpp"
#include "gui/position.hpp"
#include "gamestate.hpp"
#include "renderer.hpp"

using namespace trc;

namespace {

struct LayoutMetrics {
    int SidebarWidth;
    int SidebarHeight;
    int ChatWidth;
    int ChatHeight;
    int GameWidth;
    int GameHeight;
    int GamestateX;
    int GamestateY;
    int GamestateWidth;
    int GamestateHeight;
};

LayoutMetrics CalculateLayout(int windowWidth, int windowHeight) {
    constexpr auto SidebarWidth = 176;
    constexpr auto ChatHeight = 174;
    constexpr auto Margin = 4;
    constexpr auto Border = 1;

    const auto sidebarHeight = windowHeight;
    const auto chatWidth = std::max(0, windowWidth - SidebarWidth);
    const auto gameWidth = std::max(0, windowWidth - SidebarWidth);
    const auto gameHeight = std::max(0, windowHeight - ChatHeight);

    const auto maxWidth = std::max(0, gameWidth - ((Margin + Border) * 2));
    const auto maxHeight = std::max(0, gameHeight - ((Margin + Border) * 2));

    const auto scale = std::max(
            0.0,
            std::min(maxWidth / static_cast<double>(Renderer::NativeResolutionX),
                     maxHeight / static_cast<double>(Renderer::NativeResolutionY)));

    const auto gamestateWidth =
            static_cast<int>(Renderer::NativeResolutionX * scale);
    const auto gamestateHeight =
            static_cast<int>(Renderer::NativeResolutionY * scale);

    const auto gamestateX = ((maxWidth - gamestateWidth) / 2) + Margin + Border;
    const auto gamestateY = ((maxHeight - gamestateHeight) / 2) + Margin + Border;

    return LayoutMetrics{SidebarWidth,
                         sidebarHeight,
                         chatWidth,
                         ChatHeight,
                         gameWidth,
                         gameHeight,
                         gamestateX,
                         gamestateY,
                         gamestateWidth,
                         gamestateHeight};
}

} // namespace

void Builder::Gui::Relayout(int windowWidth, int windowHeight) {
    const auto layout = CalculateLayout(windowWidth, windowHeight);

    Root->SetLayoutSize(windowWidth, windowHeight);

    if (Root->Sidebar != nullptr) {
        Root->Sidebar->SetLayoutSize(layout.SidebarWidth, layout.SidebarHeight);
        Root->SetChildPosition(Root->Sidebar,
                               gui::Position(windowWidth - layout.SidebarWidth,
                                             0));
    }

    if (Root->Chat != nullptr) {
        Root->Chat->SetLayoutSize(layout.ChatWidth, layout.ChatHeight);
        Root->SetChildPosition(Root->Chat,
                               gui::Position(0,
                                             windowHeight - layout.ChatHeight));
    }

    if (Root->Game != nullptr) {
        Root->Game->SetLayoutSize(layout.GameWidth, layout.GameHeight);
        Root->Game->SetGamestateBounds(layout.GamestateX,
                                       layout.GamestateY,
                                       layout.GamestateWidth,
                                       layout.GamestateHeight);
    }

    GamestateX = layout.GamestateX;
    GamestateY = layout.GamestateY;
    GamestateWidth = layout.GamestateWidth;
    GamestateHeight = layout.GamestateHeight;
}

std::unique_ptr<Builder::Gui> Builder::BuildGui(int windowWidth,
                                                int windowHeight,
                                                trc::Gamestate *gamestate,
                                                GuiState *guiState) {
    const auto layout = CalculateLayout(windowWidth, windowHeight);

    auto root = std::make_unique<Builder::RootPanel>(windowWidth, windowHeight);

    auto sidebar = Builder::BuildSidebar(layout.SidebarHeight, gamestate, guiState);
    root->Sidebar = sidebar.get();
    root->Add(std::move(sidebar),
              gui::Position(windowWidth - layout.SidebarWidth, 0));

    auto chat = Builder::BuildChat(layout.ChatWidth,
                                   layout.ChatHeight,
                                   gamestate);
    root->Chat = chat.get();
    root->Add(std::move(chat),
              gui::Position(0, windowHeight - layout.ChatHeight));

    auto game = Builder::BuildGame(layout.GameWidth,
                                   layout.GameHeight,
                                   gamestate,
                                   layout.GamestateX,
                                   layout.GamestateY,
                                   layout.GamestateWidth,
                                   layout.GamestateHeight);
    root->Game = game.get();
    root->Add(std::move(game), gui::Position(0, 0));

    return std::make_unique<Builder::Gui>(std::move(root),
                                          layout.GamestateX,
                                          layout.GamestateY,
                                          layout.GamestateWidth,
                                          layout.GamestateHeight);
}
