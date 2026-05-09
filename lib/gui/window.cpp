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

#include "gui/window.hpp"

#include <iostream>
#include <string>

#include "gui/button.hpp"
#include "gui/common.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"
#include "gui/widget.hpp"

#include "canvas.hpp"
#include "pixel.hpp"
#include "sprites.hpp"
#include "textrenderer.hpp"
#include "versions.hpp"

namespace trc {
namespace gui {

Window::Window(int width,
               int height,
               const Version *version,
               Type type,
               const Sprite *icon,
               const std::string &title,
               const OnClickHandler &closeOnClick)
    : Widget(width, height),
      _Version(version),
      WindowType(type),
      Icon(icon),
      Title(title),
      CloseOnClick(closeOnClick),
      Content(nullptr),
      ContentCanvas(nullptr),
      ScrollOffset(0),
      MaximizedHeight(height),
      MinimizeButton(&version->Icons.Minimize,
                     &version->Icons.MinimizePressed,
                     Button::ButtonType::Toggle),
      MinimizeButtonPosition(Position(width - 28, 2)),
      CloseButton(&version->Icons.Close,
                  &version->Icons.ClosePressed,
                  Button::ButtonType::Normal),
      CloseButtonPosition(Position(width - 15, 2)) {
    MinHeight = 57;
    MaxHeight = height;
    MinimizeButton.SetOnClick([this]() { MinimizeOnClick(); });
    CloseButton.SetOnClick(CloseOnClick);
}

void Window::Update(State &state, Position offset) {
    if (Content) {
        Content->Update(state, offset + Position(4, 15));

        // Re-create content canvas if content size has changed
        if (!ContentCanvas || (Content->Width != ContentCanvas->Width ||
                               Content->Height != ContentCanvas->Height)) {
            ContentCanvas =
                    std::make_unique<Canvas>(Content->Width, Content->Height);
        }

        // Set MaxHeight based on content size
        MaxHeight = Content->Height + 19;
    }

    MinimizeButton.Update(state, offset + MinimizeButtonPosition);
    CloseButton.Update(state, offset + CloseButtonPosition);

    if (!MinimizeButton.Toggled &&
        PointInsideWidget(state.MousePosition(offset), *this) &&
        state.MousePosition(offset).Y >= Height - 19) {
        state.RequestMouseCursor(State::MouseCursor::Resize);
    }
}

void Window::Render(Canvas &canvas, Position offset) {
    const auto &icons = _Version->Icons;
    const auto &fonts = _Version->Fonts;

    // Header
    canvas.Draw(icons.WindowHeaderLeft, offset.X, offset.Y);
    canvas.DrawBackground(icons.WindowHeaderMiddle,
                          offset.X + 4,
                          offset.Y,
                          offset.X + Width - 4,
                          offset.Y + 15);
    canvas.Draw(icons.WindowHeaderRight, offset.X + Width - 4, offset.Y);
    canvas.Draw(*Icon, offset.X + 4, offset.Y + 2);
    TextRenderer::DrawString(fonts.InterfaceLarge,
                             Pixel(0x8F, 0x8F, 0x8F),
                             offset.X + 20,
                             offset.Y + 4,
                             Title,
                             canvas);

    MinimizeButton.Render(canvas, offset + MinimizeButtonPosition);
    CloseButton.Render(canvas, offset + CloseButtonPosition);

    if (MinimizeButton.Toggled) {
        // Bottom
        canvas.Draw(icons.WindowBottomLeft, offset.X, offset.Y + Height - 4);
        canvas.DrawBackground(icons.WindowBottom,
                              offset.X + 4,
                              offset.Y + Height - 4,
                              offset.X + Width - 4,
                              offset.Y + Height);
        canvas.Draw(icons.WindowBottomRight,
                    offset.X + Width - 4,
                    offset.Y + Height - 4);
        return;
    }

    if (Content) {
        // Render on content canvas first
        ContentCanvas->Wipe();
        Content->Render(*ContentCanvas, Position(0, 0));

        // Then render content canvas to the given canvas
        // with respect to the scrollbar
        Canvas::Copy(canvas,
                     *ContentCanvas,
                     0,
                     ScrollOffset,
                     Width - 4 - 16,
                     ScrollOffset + Height - 15 - 4,
                     offset.X + 4,
                     offset.Y + 15);

    } else {
        canvas.DrawBackground(icons.ClientBackground,
                              offset.X + 4,
                              offset.Y + 15,
                              offset.X + Width - 4,
                              offset.Y + Height - 4);
    }

    // Middle
    canvas.DrawBackground(icons.WindowLeft,
                          offset.X,
                          offset.Y + 15,
                          offset.X + 4,
                          offset.Y + 15 + Height - 19);
    canvas.DrawBackground(icons.WindowRight,
                          offset.X + Width - 4,
                          offset.Y + 15,
                          offset.X + Width,
                          offset.Y + 15 + Height - 19);

    // Scrollbar
    canvas.Draw(icons.ScrollbarUp, offset.X + Width - 16, offset.Y + 15);
    canvas.DrawBackground(icons.ScrollbarBackground,
                          offset.X + Width - 16,
                          offset.Y + 27,
                          offset.X + Width - 16 + 12,
                          offset.Y + 27 + Height - 43);
    canvas.Draw(icons.ScrollbarButton, offset.X + Width - 16, offset.Y + 27);
    canvas.Draw(icons.ScrollbarDown,
                offset.X + Width - 16,
                offset.Y + Height - 16);

    // Bottom
    canvas.Draw(icons.WindowBottomLeft, offset.X, offset.Y + 15 + Height - 19);
    canvas.DrawBackground(icons.WindowBottom,
                          offset.X + 4,
                          offset.Y + 15 + Height - 19,
                          offset.X + Width - 4,
                          offset.Y + 15 + Height - 19 + 4);
    canvas.Draw(icons.WindowBottomRight,
                offset.X + Width - 4,
                offset.Y + 15 + Height - 19);
    canvas.Draw(icons.WindowResize,
                offset.X + 3,
                offset.Y + 15 + Height - 19 - 12);
}

Widget::MouseEventResult Window::MouseLeftDown(Position position) {
    if (PointInsideWidget(position - MinimizeButtonPosition, MinimizeButton)) {
        return MinimizeButton.MouseLeftDown(position - MinimizeButtonPosition);
    }
    
    if (PointInsideWidget(position - CloseButtonPosition, CloseButton)) {
        return CloseButton.MouseLeftDown(position - CloseButtonPosition);
    }

    // If click is on the header, then start drag action
    if (position.Y < 15) {
        return Widget::MouseEventResult::StartDrag;
    }

    // If click is on the bottom, then start resize action
    if (!MinimizeButton.Toggled && position.Y >= Height - 19) {
        return Widget::MouseEventResult::Resize;
    }

    return Widget::MouseEventResult::None;
}

void Window::MouseLeftUp(Position position) {
    if (PointInsideWidget(position - MinimizeButtonPosition, MinimizeButton)) {
        MinimizeButton.MouseLeftUp(position - MinimizeButtonPosition);
    } else if (PointInsideWidget(position - CloseButtonPosition, CloseButton)) {
        CloseButton.MouseLeftUp(position - CloseButtonPosition);
    }
}

void Window::MinimizeOnClick() {
    if (MinimizeButton.Toggled) {
        Height = 19;
    } else {
        Height = MaximizedHeight;
    }
}

} // namespace gui
} // namespace trc
