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

#ifndef __TRC_GUI_BUTTON_HPP__
#define __TRC_GUI_BUTTON_HPP__

#include <functional>
#include <string>

#include "gui/position.hpp"
#include "gui/widget.hpp"

#include "pixel.hpp"
#include "sprites.hpp"

namespace trc {

struct Font;
class Canvas;

namespace gui {

struct State;

struct Button : public Widget {
    using OnClickHandler = std::function<void()>;

    const Sprite *SpriteNormal;
    const Sprite *SpritePressed;
    enum class ButtonType { Normal, Toggle } Type;
    std::string Text;
    Pixel TextColor;
    const Font *TextFont;
    OnClickHandler OnClick;

    // Whether to render normal or pressed sprite
    bool RenderPressed;

    // Left mouse has been pressed on the button, but not released yet
    bool Pressed;

    // Only used when Type is Toggle
    bool Toggled;

    Button(const Sprite *spriteNormal,
           const Sprite *spritePressed,
           ButtonType type,
           const OnClickHandler &onClick)
        : Button(spriteNormal,
                 spritePressed,
                 type,
                 "",
                 Pixel(0, 0, 0),
                 nullptr,
                 onClick) {
    }

    Button(const Sprite *spriteNormal,
           const Sprite *spritePressed,
           ButtonType type,
           const std::string &text,
           const Pixel &textColor,
           const Font *textFont,
           const OnClickHandler &onClick)
        : Widget(spriteNormal->Width, spriteNormal->Height),
          SpriteNormal(spriteNormal),
          SpritePressed(spritePressed),
          Type(type),
          Text(text),
          TextColor(textColor),
          TextFont(textFont),
          OnClick(onClick),
          RenderPressed(false),
          Pressed(false),
          Toggled(false) {
    }

    void Update(State &state, Position offset) override;
    void Render(Canvas &canvas, Position offset) override;

    MouseEventResult MouseLeftDown(Position position) override;
    void MouseLeftUp(Position position) override;
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_BUTTON_HPP__

