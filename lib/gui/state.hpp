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

#ifndef __TRC_GUI_STATE_HPP__
#define __TRC_GUI_STATE_HPP__

#include "gui/position.hpp"

namespace trc {
namespace gui {

// This interface should be implemented and passed to Gui::Render to allow
// widgets to query the current input state
struct State {
    enum class MouseCursor {
        Default,
        Resize,
    };

    virtual Position MousePosition() const = 0;
    virtual bool MouseLeftDown() const = 0;
    virtual void RequestMouseCursor(MouseCursor cursor) = 0;
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_STATE_HPP__

