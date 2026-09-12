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
        RenderSunkenBorder(*_Icons, canvas, offset, Width, Height);
        break;
    case BorderType::Raised:
        RenderRaisedBorder(*_Icons, canvas, offset, Width, Height);
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

void Border::RenderSunkenBorder(const Icons &icons, Canvas &canvas, Position offset, int width, int height) {
    canvas.DrawTiled(icons.BorderHorizontalDark,
                     offset.X,
                     offset.Y,
                     offset.X + width,
                     offset.Y + 1);
    canvas.DrawTiled(icons.BorderVerticalDark,
                     offset.X,
                     offset.Y + 1,
                     offset.X + 1,
                     offset.Y + height - 1);
    canvas.DrawTiled(icons.BorderVerticalLight,
                     offset.X + width - 1,
                     offset.Y + 1,
                     offset.X + width,
                     offset.Y + height - 1);
    canvas.DrawTiled(icons.BorderHorizontalLight,
                     offset.X,
                     offset.Y + height - 1,
                     offset.X + width,
                     offset.Y + height);
}

void Border::RenderRaisedBorder(const Icons &icons, Canvas &canvas, Position offset, int width, int height) {
    canvas.DrawTiled(icons.BorderCornerLight,
                     offset.X,
                     offset.Y,
                     offset.X + 2,
                     offset.Y + 2);
    canvas.DrawTiled(icons.BorderHorizontalLight,
                     offset.X + 2,
                     offset.Y,
                     offset.X + width - 2,
                     offset.Y + 2);
    canvas.DrawTiled(icons.BorderCornerLightDark,
                     offset.X + width - 2,
                     offset.Y,
                     offset.X + width,
                     offset.Y + 2);
    canvas.DrawTiled(icons.BorderVerticalLight,
                     offset.X,
                     offset.Y + 2,
                     offset.X + 2,
                     offset.Y + height - 2);
    canvas.DrawTiled(icons.BorderVerticalDark,
                     offset.X + width - 2,
                     offset.Y + 2,
                     offset.X + width,
                     offset.Y + height - 2);
    canvas.DrawTiled(icons.BorderCornerLightDark,
                     offset.X,
                     offset.Y + height - 2,
                     offset.X + 2,
                     offset.Y + height);
    canvas.DrawTiled(icons.BorderHorizontalDark,
                     offset.X + 2,
                     offset.Y + height - 2,
                     offset.X + width - 2,
                     offset.Y + height);
    canvas.DrawTiled(icons.BorderCornerDark,
                     offset.X + width - 2,
                     offset.Y + height - 2,
                     offset.X + width,
                     offset.Y + height);
}

} // namespace gui
} // namespace trc
