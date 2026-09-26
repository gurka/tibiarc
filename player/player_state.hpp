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

#ifndef __TRC_PLAYER_STATE_HPP__
#define __TRC_PLAYER_STATE_HPP__

#include "gui/state.hpp"
#include "gui/position.hpp"

using MouseCursor = trc::gui::State::MouseCursor;

struct GuiState : public trc::gui::State {
    bool SkillsWindowVisible = true;
    bool BattleWindowVisible = true;
    bool VIPWindowVisible = false;

    void SetMousePosition(int x, int y) {
        MouseX = x;
        MouseY = y;
    }

    void SetMouseLeftDown(bool down) {
        _MouseLeftDown = down;
    }

    void ResetRequestedCursor() {
        RequestedCursor = MouseCursor::Default;
    }

    // Returns the cursor that was requested since the last ResetRequestedCursor(),
    // and whether it differs from the currently active cursor.
    bool ConsumeCursorChange(MouseCursor &cursor) {
        if (RequestedCursor == CurrentCursor) {
            return false;
        }
        cursor = RequestedCursor;
        CurrentCursor = RequestedCursor;
        return true;
    }

    trc::gui::Position MousePosition(trc::gui::Position offset) const override {
        return trc::gui::Position(MouseX, MouseY) - offset;
    }

    bool MouseLeftDown() const override {
        return _MouseLeftDown;
    }

    void RequestMouseCursor(MouseCursor cursor) override {
        RequestedCursor = cursor;
    }

private:
    int MouseX = 0;
    int MouseY = 0;
    bool _MouseLeftDown = false;
    MouseCursor CurrentCursor = MouseCursor::Default;
    MouseCursor RequestedCursor = MouseCursor::Default;
};

#endif // __TRC_PLAYER_STATE_HPP__
