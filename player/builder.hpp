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

struct GamePanel;

struct RootPanel : public trc::gui::Panel {
    trc::gui::Widget *Sidebar = nullptr;
    trc::gui::Widget *Chat = nullptr;
    GamePanel *Game = nullptr;

    RootPanel(int width, int height)
        : Panel(width, height) {
    }
};

struct Gui {
    std::unique_ptr<RootPanel> Root;

    Gui(std::unique_ptr<RootPanel> root, int gamestateX, int gamestateY, int gamestateWidth, int gamestateHeight)
        : Root(std::move(root)),
          GamestateX(gamestateX),
          GamestateY(gamestateY),
          GamestateWidth(gamestateWidth),
          GamestateHeight(gamestateHeight) {
    }

    int GetGamestateX() const { return GamestateX; }
    int GetGamestateY() const { return GamestateY; }
    int GetGamestateWidth() const { return GamestateWidth; }
    int GetGamestateHeight() const { return GamestateHeight; }

    void Relayout(int windowWidth, int windowHeight);

private:
    int GamestateX;
    int GamestateY;
    int GamestateWidth;
    int GamestateHeight;
};

std::unique_ptr<Gui> BuildGui(int windowWidth,
                              int windowHeight,
                              trc::Gamestate *gamestate,
                              GuiState *guiState);

} // namespace Builder

#endif // __TRC_GUI_TEST_GUI_BUILDER_HPP__
