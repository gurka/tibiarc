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

#ifndef __TRC_GUI_POSITION_HPP__
#define __TRC_GUI_POSITION_HPP__

namespace trc {
namespace gui {

struct Position {
    int X;
    int Y;

    Position(int x, int y)
        : X(x), Y(y) {
    }

    Position operator+(const Position &other) const {
        return Position(X + other.X, Y + other.Y);
    }

    Position &operator+=(const Position &other) {
        X += other.X;
        Y += other.Y;
        return *this;
    }

    Position operator-(const Position &other) const {
        return Position(X - other.X, Y - other.Y);
    }

    Position &operator-=(const Position &other) {
        X -= other.X;
        Y -= other.Y;
        return *this;
    }
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_POSITION_HPP__

