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

#include <algorithm>
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
      ScrollUpButton(&version->Icons.ScrollbarUp,
                     &version->Icons.ScrollbarUpPressed),
      ScrollUpButtonPosition(Position(Width - 16, 15)),
      ScrollDownButton(&version->Icons.ScrollbarDown,
                       &version->Icons.ScrollbarDownPressed),
      ScrollDownButtonPosition(Position(Width - 16, Height - 16)),
      MaximizedHeight(height),
      MinimizeButton(&version->Icons.Minimize,
                     &version->Icons.MinimizePressed),
      MinimizeButtonPosition(Position(width - 28, 2)),
      CloseButton(&version->Icons.Close, &version->Icons.ClosePressed),
      CloseButtonPosition(Position(width - 15, 2)) {
    MinHeight = 57;
    MaxHeight = WindowType == Type::SidebarNoMaxHeight ? 65536 : height;
    ScrollUpButton.SetOnClick(
            [this]() { ScrollOffset = std::max(0, ScrollOffset - 10); });
    ScrollDownButton.SetOnClick(
            [this]() { ScrollOffset = std::max(0, std::min(Content->Height - Height, ScrollOffset + 10)); });
    MinimizeButton.SetOnClick([this]() { MinimizeOnClick(); });
    CloseButton.SetOnClick(CloseOnClick);
}

void Window::SetWidth(int w) {
    Widget::SetWidth(w);

    MinimizeButtonPosition = Position(Width - 28, 2);
    CloseButtonPosition = Position(Width - 15, 2);
    ScrollUpButtonPosition = Position(Width - 16, 15);
    ScrollDownButtonPosition = Position(Width - 16, Height - 16);

    if (Content) {
        Content->SetWidth(std::max(0, Width - 8));
    }
}

void Window::SetHeight(int h) {
    Widget::SetHeight(h);

    ScrollDownButtonPosition = Position(Width - 16, Height - 16);

    // We do NOT want to propagate the new height to content
    // as the content height is determined by its own content, not the window
    // size
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

        if (WindowType == Type::Sidebar) {
            // Set MaxHeight based on content size
            MaxHeight = Content->Height + 19;
        }
    }

    ScrollUpButton.Update(state, offset + ScrollUpButtonPosition);
    ScrollDownButton.Update(state, offset + ScrollDownButtonPosition);
    MinimizeButton.Update(state, offset + MinimizeButtonPosition);
    CloseButton.Update(state, offset + CloseButtonPosition);

    if (!MinimizeButton.Toggled &&
        PointInsideWidget(state.MousePosition(offset), *this) &&
        state.MousePosition(offset).Y >= Height - 4) {
        state.RequestMouseCursor(State::MouseCursor::Resize);
    }
}

void Window::Render(Canvas &canvas, Position offset) {
    const auto &icons = _Version->Icons;
    const auto &fonts = _Version->Fonts;

    // Header
    canvas.Draw(icons.WindowHeaderLeft, offset.X, offset.Y);
    canvas.DrawTiled(icons.WindowHeaderMiddle,
                     offset.X + 4,
                     offset.Y,
                     offset.X + Width - 4,
                     offset.Y + 15);
    canvas.Draw(icons.WindowHeaderRight, offset.X + Width - 4, offset.Y);
    if (Icon->Width != 12 || Icon->Height != 12) {
        canvas.DrawScaled(*Icon, offset.X + 4, offset.Y + 2, 12, 12);
    } else {
        canvas.Draw(*Icon, offset.X + 4, offset.Y + 2);
    }
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
        canvas.DrawTiled(icons.WindowBottom,
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
        if (Content->Visible && Content->Height > 0) {
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
        }

        if (WindowType == Type::SidebarNoMaxHeight && (!Content->Visible || Content->Height < Height - 15 - 4)) {
            // If the content is smaller than the window, then fill the rest
            // with background
            // Note: this will only look good if the content uses the same background as the window
            canvas.DrawTiled(icons.ClientBackground,
                             offset.X + 4,
                             offset.Y + 15 + (Content->Visible ? Content->Height : 0),
                             offset.X + Width - 4,
                             offset.Y + Height - 4);

        }
    } else {
        canvas.DrawTiled(icons.ClientBackground,
                         offset.X + 4,
                         offset.Y + 15,
                         offset.X + Width - 4,
                         offset.Y + Height - 4);
    }

    // Middle
    canvas.DrawTiled(icons.WindowLeft,
                     offset.X,
                     offset.Y + 15,
                     offset.X + 4,
                     offset.Y + 15 + Height - 19);
    canvas.DrawTiled(icons.WindowRight,
                     offset.X + Width - 4,
                     offset.Y + 15,
                     offset.X + Width,
                     offset.Y + 15 + Height - 19);

    // Scrollbar
    ScrollUpButton.Render(canvas, offset + ScrollUpButtonPosition);
    canvas.DrawTiled(icons.ScrollbarBackground,
                     offset.X + Width - 16,
                     offset.Y + 27,
                     offset.X + Width - 16 + 12,
                     offset.Y + 27 + Height - 43);

    /*
    // Scrollbar: calculate how much of the content is visible in the window
    int contentHeight = Height - 15 - 4;
    float visibleContentHeight = static_cast<float>(contentHeight) / std::min(Content->Height, contentHeight);
    int scrollbarHeight = Height - 43;
    int scrollbarButtonHeight =
            static_cast<int>(scrollbarHeight * visibleContentHeight);
    int scrollbarButtonOffset = static_cast<int>(
            scrollbarHeight * (static_cast<float>(ScrollOffset) /
                               std::max(1, Content->Height - contentHeight)));
    canvas.Draw(icons.ScrollbarButton,
                offset.X + Width - 16,
                offset.Y + 27 + scrollbarButtonOffset,
                12,
                4,
                0,
                0,
                0,
                0);
    canvas.Draw(icons.ScrollbarButton,
                offset.X + Width - 16,
                offset.Y + 27 + scrollbarButtonOffset + 4,
                12,
                scrollbarButtonHeight - 8,
                0,
                4,
                0,
                4);
    */


    canvas.Draw(icons.ScrollbarButton, offset.X + Width - 16, offset.Y + 27);
    ScrollDownButton.Render(canvas, offset + ScrollDownButtonPosition);

    // Bottom
    canvas.Draw(icons.WindowBottomLeft, offset.X, offset.Y + 15 + Height - 19);
    canvas.DrawTiled(icons.WindowBottom,
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
    PressedButton = nullptr;

    if (PointInsideWidget(position - ScrollUpButtonPosition, ScrollUpButton)) {
        PressedButton = &ScrollUpButton;
        return ScrollUpButton.MouseLeftDown(position - ScrollUpButtonPosition);
    }

    if (PointInsideWidget(position - ScrollDownButtonPosition,
                          ScrollDownButton)) {
        PressedButton = &ScrollDownButton;
        return ScrollDownButton.MouseLeftDown(position -
                                              ScrollDownButtonPosition);
    }

    if (PointInsideWidget(position - MinimizeButtonPosition, MinimizeButton)) {
        PressedButton = &MinimizeButton;
        return MinimizeButton.MouseLeftDown(position - MinimizeButtonPosition);
    }

    if (PointInsideWidget(position - CloseButtonPosition, CloseButton)) {
        PressedButton = &CloseButton;
        return CloseButton.MouseLeftDown(position - CloseButtonPosition);
    }

    // If click is on the header, then start drag action
    if (position.Y < 15) {
        return Widget::MouseEventResult::StartDrag;
    }

    // If click is on the bottom, then start resize action
    if (!MinimizeButton.Toggled && position.Y >= Height - 4) {
        return Widget::MouseEventResult::Resize;
    }

    return Widget::MouseEventResult::NotHandled;
}

void Window::MouseLeftUp(Position position) {
    if (PressedButton != nullptr) {
        const auto &pos = [this]() {
            if (PressedButton == &ScrollUpButton)
                return ScrollUpButtonPosition;
            if (PressedButton == &ScrollDownButton)
                return ScrollDownButtonPosition;
            if (PressedButton == &MinimizeButton)
                return MinimizeButtonPosition;
            else // CloseButton
                return CloseButtonPosition;
        }();
        PressedButton->MouseLeftUp(position - pos);
        PressedButton = nullptr;
    }
}

void Window::MinimizeOnClick() {
    if (MinimizeButton.Toggled) {
        MaximizedHeight = Height;
        Height = 19;
    } else {
        Height = MaximizedHeight;
    }
}

} // namespace gui
} // namespace trc
