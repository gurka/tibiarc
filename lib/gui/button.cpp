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

#include "button.hpp"
#include "common.hpp"
#include "state.hpp"
#include "textrenderer.hpp"

namespace trc {
namespace gui {

void Button::Render(Canvas &canvas, const State &state) {
    // Reset Pressed if the left mouse button is longer down
    if (!state.MouseLeftDown()) {
        Pressed = false;
    }

    // Render
    int textOffset = 0;
    if ((Pressed && MouseIsOverWidget(*this, state.MouseX(), state.MouseY())) || (Type == ButtonType::Toggle && Toggled)) {
        canvas.Draw(*SpritePressed, X, Y);
        textOffset = 1;
    } else {
        canvas.Draw(*SpriteNormal, X, Y);
    }

    if (TextFont != nullptr) {
        TextRenderer::DrawCenteredString(
                *TextFont,
                TextColor,
                X + (SpriteNormal->Width / 2) + textOffset,
                Y + (SpriteNormal->Height / 2) - (TextFont->Height / 2) +
                        textOffset,
                Text,
                canvas);
    }
}

void Button::MouseLeftDown(int x, int y) {
    Pressed = true;
}

void Button::MouseLeftUp(int x, int y) {
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
