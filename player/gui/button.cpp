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
    RenderPressed =
            IsPressed && PointInsideWidget(state.MousePosition(offset), *this);
}

void Button::Render(Canvas &canvas, Position offset) {
    int textOffset = RenderPressed ? 1 : 0;
    canvas.Draw(*(RenderPressed ? Pressed : Normal), offset.X, offset.Y);

    if (TextFont != nullptr) {
        TextRenderer::DrawCenteredString(
                *TextFont,
                TextColor,
                offset.X + (Normal->Width / 2) + textOffset,
                offset.Y + (Normal->Height / 2) - (TextFont->Height / 2) +
                        textOffset,
                Text,
                canvas);
    }
}

Widget::MouseEventResult Button::OnMouseEvent(MouseEvent event, Position position) {
    if (event == MouseEvent::LeftDown) {
        IsPressed = true;
        return Widget::MouseEventResult::Handled;
    } else if (event == MouseEvent::LeftUp) {
        if (IsPressed) {
            IsPressed = false;
            if (PointInsideWidget(position, *this)) {
                HandleClick();
            }
        }
        return Widget::MouseEventResult::Handled;
    }
    return Widget::MouseEventResult::NotHandled;
}

void Button::HandleClick() {
    if (OnClick) {
        OnClick();
    }
}

void ToggleButton::Update(State &state, Position offset) {
    Button::Update(state, offset);
    RenderPressed = RenderPressed || (Toggled && ToggledPressed == nullptr);
}

void ToggleButton::Render(Canvas &canvas, Position offset) {
    const Sprite *sprite = nullptr;
    if (Toggled) {
        if (RenderPressed && ToggledPressed != nullptr) {
            sprite = ToggledPressed;
        } else if (!RenderPressed && ToggledNormal != nullptr) {
            sprite = ToggledNormal;
        }
    }

    if (sprite == nullptr) {
        sprite = RenderPressed ? Pressed : Normal;
    }

    int textOffset = RenderPressed ? 1 : 0;
    canvas.Draw(*sprite, offset.X, offset.Y);

    if (TextFont != nullptr) {
        TextRenderer::DrawCenteredString(
                *TextFont,
                TextColor,
                offset.X + (Normal->Width / 2) + textOffset,
                offset.Y + (Normal->Height / 2) - (TextFont->Height / 2) +
                        textOffset,
                Text,
                canvas);
    }
}

void ToggleButton::HandleClick() {
    Toggled = !Toggled;
    Button::HandleClick();
}

} // namespace gui
} // namespace trc
