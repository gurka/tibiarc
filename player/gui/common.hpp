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

#include <algorithm>
#include <concepts>
#include <memory>

#include "gui/position.hpp"
#include "gui/widget.hpp"

namespace trc {
namespace gui {

template <std::derived_from<Widget> T = Widget>
struct PlacedWidget {
    PlacedWidget() : PlacedWidget(nullptr, trc::gui::Position(0, 0)) {
    }

    PlacedWidget(std::unique_ptr<T> widget, Position position)
        : Widget(std::move(widget)), Position(position) {
    }

    std::unique_ptr<T> Widget;
    Position Position;
};

// Tracks the in-progress drag of a widget inside a container.
struct DragState {
    Widget *Target = nullptr;
    Position CurrentPosition{0, 0};
    Position InitialPosition{0, 0};
    Position MouseInitialPosition{0, 0};

    bool Active() const {
        return Target != nullptr;
    }

    void Begin(Widget *target, Position widgetPos, Position mousePos) {
        Target = target;
        CurrentPosition = widgetPos;
        InitialPosition = widgetPos;
        MouseInitialPosition = mousePos;
    }

    // Recomputes CurrentPosition. Clears Target if the mouse button was
    // released. Returns true while a drag is still active.
    bool Update(bool mouseDown, Position mousePos, int containerW, int containerH) {
        if (!Active()) {
            return false;
        }
        if (!mouseDown) {
            Target = nullptr;
            return false;
        }
        CurrentPosition = InitialPosition + mousePos - MouseInitialPosition;
        CurrentPosition.X = std::clamp(CurrentPosition.X, 0, containerW - Target->Width);
        CurrentPosition.Y = std::clamp(CurrentPosition.Y, 0, containerH - Target->Height);
        return true;
    }
};

// Tracks the in-progress resize of a widget inside a container.
struct ResizeState {
    Widget *Target = nullptr;
    int InitialHeight = 0;
    Position MouseInitialPosition{0, 0};

    bool Active() const {
        return Target != nullptr;
    }

    void Begin(Widget *target, Position mousePos) {
        Target = target;
        InitialHeight = target->Height;
        MouseInitialPosition = mousePos;
    }

    // Updates Target->Height. Clears Target if the mouse button was released.
    // maxHeightFromPanel is the remaining panel space below the widget's top edge.
    void Update(bool mouseDown, Position mousePos, int maxHeightFromPanel) {
        if (!Active()) {
            return;
        }
        if (!mouseDown) {
            Target = nullptr;
            return;
        }
        Target->SetHeight(
                std::clamp(InitialHeight + mousePos.Y - MouseInitialPosition.Y,
                           Target->MinHeight,
                           std::min(Target->MaxHeight, maxHeightFromPanel)));
    }
};

inline bool PointInsideWidget(Position position, const Widget &widget) {
    // Note: assumes that position is relative to the widget
    return position.X >= 0 && position.X < widget.Width && position.Y >= 0 &&
           position.Y < widget.Height;
}

inline bool PointInsideWidget(Position position, const PlacedWidget<> &pw) {
    return position.X >= pw.Position.X &&
           position.X < pw.Position.X + pw.Widget->Width &&
           position.Y >= pw.Position.Y &&
           position.Y < pw.Position.Y + pw.Widget->Height;
}

inline bool PointInsideArea(Position position,
                            Position area,
                            int areaWidth,
                            int areaHeight) {
    return position.X >= area.X && position.X < area.X + areaWidth &&
           position.Y >= area.Y && position.Y < area.Y + areaHeight;
}

inline bool WidgetsIntersect(const Widget &wa,
                             Position pa,
                             const Widget &wb,
                             Position pb) {
    return pa.X < pb.X + wb.Width && pa.X + wa.Width > pb.X &&
           pa.Y < pb.Y + wb.Height && pa.Y + wa.Height > pb.Y;
}

inline bool WidgetsIntersect(const PlacedWidget<> &pwa, const PlacedWidget<> &pwb) {
    return WidgetsIntersect(*pwa.Widget, pwa.Position, *pwb.Widget, pwb.Position);
}

inline bool AreasIntersect(Position ap,
                           int aw,
                           int ah,
                           Position bp,
                           int bw,
                           int bh) {
    return ap.X < bp.X + bw && ap.X + aw > bp.X && ap.Y < bp.Y + bh &&
           ap.Y + ah > bp.Y;
}

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_COMMON_HPP__

