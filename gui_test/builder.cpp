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
#include "state.hpp"

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
    auto *rootPanel = dynamic_cast<gui::Panel *>(Root.get());
    if (rootPanel == nullptr) {
        return;
    }

    const auto layout = CalculateLayout(windowWidth, windowHeight);

    Root->SetSize(windowWidth, windowHeight);

    if (Sidebar != nullptr) {
        Sidebar->SetSize(layout.SidebarWidth, layout.SidebarHeight);
        rootPanel->SetChildPosition(Sidebar,
                                    gui::Position(windowWidth - layout.SidebarWidth,
                                                  0));
    }

    if (Chat != nullptr) {
        Chat->SetSize(layout.ChatWidth, layout.ChatHeight);
        rootPanel->SetChildPosition(Chat,
                                    gui::Position(0,
                                                  windowHeight - layout.ChatHeight));
    }

    if (ChatContent != nullptr) {
        ChatContent->SetSize(layout.ChatWidth, layout.ChatHeight);
    }

    if (Game != nullptr) {
        Game->SetSize(layout.GameWidth, layout.GameHeight);
    }

    if (GamestateWidget != nullptr) {
        GamestateWidget->SetSize(layout.GamestateWidth, layout.GamestateHeight);
    }

    if (GameBorder != nullptr) {
        auto *gamePanel = dynamic_cast<gui::Panel *>(Game);
        if (gamePanel != nullptr) {
            gamePanel->SetChildPosition(GameBorder,
                                        gui::Position(layout.GamestateX - 1,
                                                      layout.GamestateY - 1));
        }
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

    auto panel = std::make_unique<gui::Panel>(windowWidth, windowHeight);

    auto sidebar = Builder::BuildSidebar(layout.SidebarHeight, gamestate, guiState);
    auto *sidebarPtr = sidebar.get();
    panel->Add(std::move(sidebar),
               gui::Position(windowWidth - layout.SidebarWidth, 0));

    gui::Widget *chatContent = nullptr;
    auto chat = Builder::BuildChat(layout.ChatWidth,
                                   layout.ChatHeight,
                                   gamestate,
                                   guiState,
                                   &chatContent);
    auto *chatPtr = chat.get();
    panel->Add(std::move(chat),
               gui::Position(0, windowHeight - layout.ChatHeight));

    gui::Widget *gameBorder = nullptr;
    gui::Widget *gamestateWidget = nullptr;
    auto game = Builder::BuildGame(layout.GameWidth,
                                   layout.GameHeight,
                                   gamestate,
                                   guiState,
                                   layout.GamestateX,
                                   layout.GamestateY,
                                   layout.GamestateWidth,
                                   layout.GamestateHeight,
                                   &gameBorder,
                                   &gamestateWidget);
    auto *gamePtr = game.get();
    panel->Add(std::move(game), gui::Position(0, 0));

    auto gui = std::make_unique<Builder::Gui>(Builder::Gui{std::move(panel),
                                                           sidebarPtr,
                                                           chatPtr,
                                                           chatContent,
                                                           gamePtr,
                                                           gameBorder,
                                                           gamestateWidget,
                                                           layout.GamestateX,
                                                           layout.GamestateY,
                                                           layout.GamestateWidth,
                                                           layout.GamestateHeight});

    return gui;
}
