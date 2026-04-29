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

#ifndef __TRC_GUI_COMMON_HPP__
#define __TRC_GUI_COMMON_HPP__

#include <memory>
#include <tuple>

#include "gui/position.hpp"
#include "gui/widget.hpp"

namespace trc {
namespace gui {

using WidgetAndPosition = std::tuple<std::unique_ptr<Widget>, Position>;

inline bool PointInsideWidget(Position position, const Widget &widget) {
    // Note: assumes that position is relative to the widget
    return position.X >= 0 && position.X < widget.Width && position.Y >= 0 &&
           position.Y < widget.Height;
}

inline bool PointInsideWidget(Position position, const WidgetAndPosition &wap) {
    const auto &widget = *std::get<0>(wap);
    const auto &widgetPosition = std::get<1>(wap);
    return position.X >= widgetPosition.X &&
           position.X < widgetPosition.X + widget.Width &&
           position.Y >= widgetPosition.Y &&
           position.Y < widgetPosition.Y + widget.Height;
}

inline bool WidgetsIntersect(const Widget &wa,
                             Position pa,
                             const Widget &wb,
                             Position pb) {
    return pa.X < pb.X + wb.Width && pa.X + wa.Width > pb.X &&
           pa.Y < pb.Y + wb.Height && pa.Y + wa.Height > pb.Y;
}

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_COMMON_HPP__

