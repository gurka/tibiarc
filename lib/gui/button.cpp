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

#include "gui/button.hpp"

#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"
#include "gui/widget.hpp"

#include "canvas.hpp"
#include "textrenderer.hpp"

namespace trc {
namespace gui {

void Button::Update(State &state, Position offset) {
    if (!state.MouseLeftDown()) {
        Pressed = false;
    }

    RenderPressed =
            (Pressed &&
             PointInsideWidget(state.MousePosition(offset), *this)) ||
            (Type == ButtonType::Toggle && Toggled);
}

void Button::Render(Canvas &canvas, Position offset) {
    int textOffset = 0;
    if (RenderPressed) {
        canvas.Draw(*SpritePressed, offset.X, offset.Y);
        textOffset = 1;
    } else {
        canvas.Draw(*SpriteNormal, offset.X, offset.Y);
    }

    if (TextFont != nullptr) {
        TextRenderer::DrawCenteredString(
                *TextFont,
                TextColor,
                offset.X + (SpriteNormal->Width / 2) + textOffset,
                offset.Y + (SpriteNormal->Height / 2) - (TextFont->Height / 2) +
                        textOffset,
                Text,
                canvas);
    }
}

Widget::MouseEventResult Button::MouseLeftDown(Position position) {
    Pressed = true;
    return Widget::MouseEventResult::Clicked;
}

void Button::MouseLeftUp(Position position) {
    // Only trigger the click action if the mouse was both pressed and released
    // on this button
    if (Pressed) {
        if (Type == ButtonType::Toggle) {
            Toggled = !Toggled;
        }
        OnClick();
    }

    // Pressed is reset in Render()
}

} // namespace gui
} // namespace trc
