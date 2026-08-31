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

#ifndef __TRC_GUI_TEST_GUI_BUILDER_HPP__
#define __TRC_GUI_TEST_GUI_BUILDER_HPP__

#include <memory>

#include "gui/panel.hpp"

struct GuiState;

namespace trc {
struct Gamestate;
}

namespace Builder {

struct ChatPanel;
struct GamePanel;

struct RootPanel : public trc::gui::Panel {
    trc::gui::Widget *Sidebar = nullptr;
    ChatPanel *Chat = nullptr;
    GamePanel *Game = nullptr;

    RootPanel(int width, int height)
        : Panel(width, height) {
    }
};

struct Gui {
    std::unique_ptr<RootPanel> Root;

    int GamestateX;
    int GamestateY;
    int GamestateWidth;
    int GamestateHeight;

    void Relayout(int windowWidth, int windowHeight);
};

std::unique_ptr<Gui> BuildGui(int windowWidth,
                              int windowHeight,
                              trc::Gamestate *gamestate,
                              GuiState *guiState);

} // namespace Builder

#endif // __TRC_GUI_TEST_GUI_BUILDER_HPP__
