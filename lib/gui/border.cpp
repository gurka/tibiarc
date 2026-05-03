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

void Border::Update(State &state, Position offset) {
    Child->Update(state, offset + Position(BorderWidth(), BorderWidth()));

    // Update the size in case the child has changed size
    Width = BorderWidth() * 2 + Child->Width;
    Height = BorderWidth() * 2 + Child->Height;
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

Border::MouseEventResult Border::MouseLeftDown(Position position) {
    if (position.X < BorderWidth() || position.X >= Width - BorderWidth() ||
        position.Y < BorderWidth() || position.Y >= Height - BorderWidth()) {
        return MouseEventResult::None;
    }
    return Child->MouseLeftDown(position -
                                Position(BorderWidth(), BorderWidth()));
}

void Border::MouseLeftUp(Position position) {
    if (position.X < BorderWidth() || position.X >= Width - BorderWidth() ||
        position.Y < BorderWidth() || position.Y >= Height - BorderWidth()) {
        return;
    }
    Child->MouseLeftUp(position - Position(BorderWidth(), BorderWidth()));
}

void Border::RenderSunkenBorder(Canvas &canvas, Position offset) {
    canvas.DrawBackground(_Icons->BorderHorizontalDark,
                          offset.X,
                          offset.Y,
                          offset.X + Width,
                          offset.Y + 1);
    canvas.DrawBackground(_Icons->BorderVerticalDark,
                          offset.X,
                          offset.Y + 1,
                          offset.X + 1,
                          offset.Y + Height - 1);
    canvas.DrawBackground(_Icons->BorderVerticalLight,
                          offset.X + Width - 1,
                          offset.Y + 1,
                          offset.X + Width,
                          offset.Y + Height - 1);
    canvas.DrawBackground(_Icons->BorderHorizontalLight,
                          offset.X,
                          offset.Y + Height - 1,
                          offset.X + Width,
                          offset.Y + Height);
}

void Border::RenderRaisedBorder(Canvas &canvas, Position offset) {
    canvas.DrawBackground(_Icons->BorderCornerLight,
                          offset.X,
                          offset.Y,
                          offset.X + 2,
                          offset.Y + 2);
    canvas.DrawBackground(_Icons->BorderHorizontalLight,
                          offset.X + 2,
                          offset.Y,
                          offset.X + Width - 2,
                          offset.Y + 2);
    canvas.DrawBackground(_Icons->BorderCornerLightDark,
                          offset.X + Width - 2,
                          offset.Y,
                          offset.X + Width,
                          offset.Y + 2);
    canvas.DrawBackground(_Icons->BorderVerticalLight,
                          offset.X,
                          offset.Y + 2,
                          offset.X + 2,
                          offset.Y + Height - 2);
    canvas.DrawBackground(_Icons->BorderVerticalDark,
                          offset.X + Width - 2,
                          offset.Y + 2,
                          offset.X + Width,
                          offset.Y + Height - 2);
    canvas.DrawBackground(_Icons->BorderCornerLightDark,
                          offset.X,
                          offset.Y + Height - 2,
                          offset.X + 2,
                          offset.Y + Height);
    canvas.DrawBackground(_Icons->BorderHorizontalDark,
                          offset.X + 2,
                          offset.Y + Height - 2,
                          offset.X + Width - 2,
                          offset.Y + Height);
    canvas.DrawBackground(_Icons->BorderCornerDark,
                          offset.X + Width - 2,
                          offset.Y + Height - 2,
                          offset.X + Width,
                          offset.Y + Height);
}

} // namespace gui
} // namespace trc
