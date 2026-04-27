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

#include "window.hpp"

#include <string>

#include "button.hpp"
#include "canvas.hpp"
#include "common.hpp"
#include "pixel.hpp"
#include "state.hpp"
#include "sprites.hpp"
#include "textrenderer.hpp"
#include "versions.hpp"
#include "widget.hpp"

namespace trc {
namespace gui {

Window::Window(int x,
               int y,
               int width,
               int height,
               const Version *version,
               Type type,
               const Sprite *icon,
               const std::string &title)
    : Widget(x, y, width, height),
      _Version(version),
      WindowType(type),
      Icon(icon),
      Title(title),
      Minimized(false),
      MinimizeButton(x + width - 28,
                     y + 2,
                     &version->Icons.Minimize,
                     &version->Icons.MinimizePressed,
                     Button::ButtonType::Toggle,
                     [this]() { Minimized = !Minimized; }),
      CloseButton(x + width - 15,
                  y + 2,
                  &version->Icons.Close,
                  &version->Icons.ClosePressed,
                  Button::ButtonType::Normal,
                  []() {}) {
}

void Window::Render(Canvas &canvas, const State &state) {
    const auto &icons = _Version->Icons;
    const auto &fonts = _Version->Fonts;

    // Header
    canvas.Draw(icons.WindowHeaderLeft, X, Y);
    canvas.DrawBackground(icons.WindowHeaderMiddle,
                          X + 4,
                          Y,
                          X + Width - 4,
                          Y + 15);
    canvas.Draw(icons.WindowHeaderRight, X + Width - 4, Y);
    canvas.Draw(*Icon, X + 4, Y + 2);
    TextRenderer::DrawString(fonts.InterfaceLarge,
                             Pixel(0x8F, 0x8F, 0x8F),
                             X + 20,
                             Y + 4,
                             Title,
                             canvas);
    
    MinimizeButton.Render(canvas, state);
    CloseButton.Render(canvas, state);

    if (Minimized) {
        return;
    }

    // Background
    canvas.DrawBackground(icons.ClientBackground,
                          X + 4,
                          Y + 15,
                          X + Width - 4,
                          Y + Height - 4);

    // Middle
    canvas.DrawBackground(icons.WindowLeft,
                          X,
                          Y + 15,
                          X + 4,
                          Y + 15 + Height - 19);
    canvas.DrawBackground(icons.WindowRight,
                          X + Width - 4,
                          Y + 15,
                          X + Width,
                          Y + 15 + Height - 19);

    // Scrollbar
    canvas.Draw(icons.ScrollbarUp, X + Width - 16, Y + 15);
    canvas.DrawBackground(icons.ScrollbarBackground,
                          X + Width - 16,
                          Y + 27,
                          X + Width - 16 + 12,
                          Y + 27 + Height - 43);
    canvas.Draw(icons.ScrollbarButton, X + Width - 16, Y + 27);
    canvas.Draw(icons.ScrollbarDown, X + Width - 16, Y + Height - 16);

    // Bottom
    canvas.Draw(icons.WindowBottomLeft, X, Y + 15 + Height - 19);
    canvas.DrawBackground(icons.WindowBottom,
                          X + 4,
                          Y + 15 + Height - 19,
                          X + Width - 4,
                          Y + 15 + Height - 19 + 4);
    canvas.Draw(icons.WindowBottomRight, X + Width - 4, Y + 15 + Height - 19);
    canvas.Draw(icons.WindowResize, X + 3, Y + 15 + Height - 19 - 12);
}

void Window::MouseLeftDown(int x, int y) {
    if (MouseIsOverWidget(MinimizeButton, x, y)) {
        MinimizeButton.MouseLeftDown(x, y);
    } else if (MouseIsOverWidget(CloseButton, x, y)) {
        CloseButton.MouseLeftDown(x, y);
    }
}

void Window::MouseLeftUp(int x, int y) {
    if (MouseIsOverWidget(MinimizeButton, x, y)) {
        MinimizeButton.MouseLeftUp(x, y);
    } else if (MouseIsOverWidget(CloseButton, x, y)) {
        CloseButton.MouseLeftUp(x, y);
    }
}

} // namespace gui
} // namespace trc
