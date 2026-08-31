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

#ifndef __TRC_GUI_TEST_GUI_BUILDER_CHAT_HPP__
#define __TRC_GUI_TEST_GUI_BUILDER_CHAT_HPP__

#include <memory>

#include "state.hpp"

#include "gui/border.hpp"
#include "gui/panel.hpp"
#include "gui/widget.hpp"
#include "gamestate.hpp"

namespace Builder {

struct ChatPanel : public trc::gui::Panel {
public:
    ChatPanel(int width,
              int height,
              std::unique_ptr<Widget> top,
              std::unique_ptr<trc::gui::Border> bottom)
        : Panel(width, height), Top(top.get()), Bottom(bottom.get()) {
        Add(std::move(top), trc::gui::Position(0, 0));
        Add(std::move(bottom), trc::gui::Position(0, Top->Height));
    }

    void SetLayoutSize(int width, int height) override;

private:
    trc::gui::Widget *Top;
    trc::gui::Border *Bottom;
};

std::unique_ptr<ChatPanel> BuildChat(int width,
                                     int height,
                                     trc::Gamestate *gamestate,
                                     GuiState *guiState);

} // namespace Builder

#endif // __TRC_GUI_TEST_GUI_BUILDER_CHAT_HPP__
