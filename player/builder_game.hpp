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

#ifndef __TRC_GUI_TEST_GUI_BUILDER_GAME_HPP__
#define __TRC_GUI_TEST_GUI_BUILDER_GAME_HPP__

#include <memory>

#include "gui/panel.hpp"

struct GuiState;

namespace trc {
struct Gamestate;
}

namespace Builder {

struct GamePanel : public trc::gui::Panel {
    GamePanel(int width, int height)
        : Panel(width, height) {
    }

    void SetGamestateBounds(int x, int y, int width, int height);

private:
    friend std::unique_ptr<GamePanel> BuildGame(int width,
                                                int height,
                                                trc::Gamestate *gamestate,
                                                int gamestateX,
                                                int gamestateY,
                                                int gamestateWidth,
                                                int gamestateHeight);

    trc::gui::Widget *GamestateBorder = nullptr;
    trc::gui::Widget *GamestateWidget = nullptr;
};

std::unique_ptr<GamePanel> BuildGame(int width,
                                     int height,
                                     trc::Gamestate *gamestate,
                                     int gamestateX,
                                     int gamestateY,
                                     int gamestateWidth,
                                     int gamestateHeight);

} // namespace Builder

#endif // __TRC_GUI_TEST_GUI_BUILDER_GAME_HPP__
