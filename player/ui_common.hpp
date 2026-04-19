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

#ifndef PLAYER_UI_COMMON_HPP_
#define PLAYER_UI_COMMON_HPP_

#include "icons.hpp"
#include "canvas.hpp"

namespace trc {
namespace UiCommon {

// Note: this draws a "sunken" border, i.e. top and left lines are dark while
//       bottom and right lines are light
void DrawBorder1px(const Icons &icons,
                   Canvas &canvas,
                   int leftX,
                   int topY,
                   int rightX,
                   int bottomY) noexcept;

// Note: this draws a "raised" border, i.e. top and left lines are light while
//       bottom and right lines are dark
void DrawBorder2px(const Icons &icons,
                   Canvas &canvas,
                   int leftX,
                   int topY,
                   int rightX,
                   int bottomY) noexcept;

} // namespace UiCommon
} // namespace trc

#endif /* PLAYER_UI_COMMON_HPP_ */
