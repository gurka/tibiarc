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

#include "border.hpp"

#include "gui/state.hpp"
#include "gui/position.hpp"
#include "icons.hpp"
#include "canvas.hpp"

namespace trc {
namespace gui {

void Border::SetWidth(int w) {
    Width = w;
    Child->SetWidth(w - BorderWidth() * 2);
}

void Border::SetHeight(int h) {
    Height = h;
    Child->SetHeight(h - BorderWidth() * 2);
}

void Border::Update(State &state, Position offset) {
    Child->Update(state, offset + Position(BorderWidth(), BorderWidth()));

    // Propagate child size changes upward (bottom-up)
    Width  = BorderWidth() * 2 + Child->GetWidth();
    Height = BorderWidth() * 2 + Child->GetHeight();
}

void Border::Render(Canvas &canvas, Position offset) {
    switch (Type) {
    case BorderType::Sunken:
        RenderSunkenBorder(canvas, offset);
        break;
    case BorderType::Raised:
        RenderRaisedBorder(canvas, offset);
        break;
    }
    Child->Render(canvas, offset + Position(BorderWidth(), BorderWidth()));
}

Border::MouseEventResult Border::OnMouseEvent(MouseEvent event, Position position) {
    const auto childPos = Position(BorderWidth(), BorderWidth());

    if (event == MouseEvent::LeftDown) {
        ChildPressed = false;
        if (position.X < BorderWidth() || position.X >= Width - BorderWidth() ||
            position.Y < BorderWidth() || position.Y >= Height - BorderWidth()) {
            return MouseEventResult::NotHandled;
        }
        const auto result = Child->OnMouseEvent(event, position - childPos);
        if (result != MouseEventResult::NotHandled) {
            ChildPressed = true;
        }
        return result;
    } else if (event == MouseEvent::LeftUp) {
        if (ChildPressed) {
            ChildPressed = false;
            Child->OnMouseEvent(event, position - childPos);
        }
        return MouseEventResult::Handled;
    } else {
        // WheelUp / WheelDown
        if (position.X < BorderWidth() || position.X >= Width - BorderWidth() ||
            position.Y < BorderWidth() || position.Y >= Height - BorderWidth()) {
            return MouseEventResult::NotHandled;
        }
        return Child->OnMouseEvent(event, position - childPos);
    }
}

void Border::RenderSunkenBorder(Canvas &canvas, Position offset) {
    canvas.DrawTiled(_Icons->BorderHorizontalDark,
                     offset.X,
                     offset.Y,
                     offset.X + Width,
                     offset.Y + 1);
    canvas.DrawTiled(_Icons->BorderVerticalDark,
                     offset.X,
                     offset.Y + 1,
                     offset.X + 1,
                     offset.Y + Height - 1);
    canvas.DrawTiled(_Icons->BorderVerticalLight,
                     offset.X + Width - 1,
                     offset.Y + 1,
                     offset.X + Width,
                     offset.Y + Height - 1);
    canvas.DrawTiled(_Icons->BorderHorizontalLight,
                     offset.X,
                     offset.Y + Height - 1,
                     offset.X + Width,
                     offset.Y + Height);
}

void Border::RenderRaisedBorder(Canvas &canvas, Position offset) {
    canvas.DrawTiled(_Icons->BorderCornerLight,
                     offset.X,
                     offset.Y,
                     offset.X + 2,
                     offset.Y + 2);
    canvas.DrawTiled(_Icons->BorderHorizontalLight,
                     offset.X + 2,
                     offset.Y,
                     offset.X + Width - 2,
                     offset.Y + 2);
    canvas.DrawTiled(_Icons->BorderCornerLightDark,
                     offset.X + Width - 2,
                     offset.Y,
                     offset.X + Width,
                     offset.Y + 2);
    canvas.DrawTiled(_Icons->BorderVerticalLight,
                     offset.X,
                     offset.Y + 2,
                     offset.X + 2,
                     offset.Y + Height - 2);
    canvas.DrawTiled(_Icons->BorderVerticalDark,
                     offset.X + Width - 2,
                     offset.Y + 2,
                     offset.X + Width,
                     offset.Y + Height - 2);
    canvas.DrawTiled(_Icons->BorderCornerLightDark,
                     offset.X,
                     offset.Y + Height - 2,
                     offset.X + 2,
                     offset.Y + Height);
    canvas.DrawTiled(_Icons->BorderHorizontalDark,
                     offset.X + 2,
                     offset.Y + Height - 2,
                     offset.X + Width - 2,
                     offset.Y + Height);
    canvas.DrawTiled(_Icons->BorderCornerDark,
                     offset.X + Width - 2,
                     offset.Y + Height - 2,
                     offset.X + Width,
                     offset.Y + Height);
}

} // namespace gui
} // namespace trc
