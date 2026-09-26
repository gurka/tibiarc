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
    : ScrollableWidget(width, height, version),
      _Version(version),
      WindowType(type),
      Icon(icon),
      Title(title),
      CloseOnClick(closeOnClick),
      Content(nullptr),
      ContentCanvas(nullptr),
      MaximizedHeight(height),
      MinimizeButton(&version->Icons.Minimize,
                     &version->Icons.MinimizePressed),
      MinimizeButtonPosition(Position(width - 28, 2)),
      CloseButton(&version->Icons.Close, &version->Icons.ClosePressed),
      CloseButtonPosition(Position(width - 15, 2)) {
    MinHeight = 57;
    MaxHeight = WindowType == Type::SidebarNoMaxHeight ? 65536 : height;
    MinimizeButton.SetOnClick([this]() { MinimizeOnClick(); });
    CloseButton.SetOnClick(CloseOnClick);
}

void Window::SetContent(std::unique_ptr<Widget> content) {
    Content = std::move(content);
    Content->SetWidth(std::max(0, Width - 8));
}

int Window::ContentViewportHeight() const {
    return std::max(0, Height - 15 - 4);
}

int Window::GetScrollContentHeight() const {
    return Content ? Content->GetHeight() : 0;
}

int Window::GetScrollViewportHeight() const {
    return Content && Content->Visible ? ContentViewportHeight() : 0;
}

Position Window::GetScrollUpButtonPosition() const {
    return Position(Width - 16, 15);
}

Position Window::GetScrollDownButtonPosition() const {
    return Position(Width - 16, Height - 16);
}

int Window::GetScrollbarTrackTop() const {
    return 27;
}

int Window::GetScrollbarTrackHeight() const {
    return std::max(0, Height - 43);
}

int Window::GetScrollbarX() const {
    return Width - 16;
}

void Window::SetWidth(int w) {
    Widget::SetWidth(w);

    MinimizeButtonPosition = Position(Width - 28, 2);
    CloseButtonPosition = Position(Width - 15, 2);

    if (Content) {
        Content->SetWidth(std::max(0, Width - 8));
    }

    ClampScrollOffset();
}

void Window::SetHeight(int h) {
    Widget::SetHeight(h);

    ClampScrollOffset();

    // We do NOT want to propagate the new height to content
    // as the content height is determined by its own content, not the window
    // size
}

void Window::Update(State &state, Position offset) {
    if (Content) {
        Content->Update(state, offset + Position(4, 15));

        // Re-create content canvas if content size has changed
        if (!ContentCanvas || (Content->GetWidth() != ContentCanvas->Width ||
                               Content->GetHeight() != ContentCanvas->Height)) {
            ContentCanvas =
                    std::make_unique<Canvas>(Content->GetWidth(), Content->GetHeight());
        }

        if (WindowType == Type::Sidebar) {
            // Set MaxHeight based on content size
            MaxHeight = Content->GetHeight() + 19;
        }
    }

    UpdateScrollbar(state, offset);

    MinimizeButton.Update(state, offset + MinimizeButtonPosition);
    CloseButton.Update(state, offset + CloseButtonPosition);

    if (!MinimizeButton.IsToggled() &&
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

    if (MinimizeButton.IsToggled()) {
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
        if (Content->Visible && Content->GetHeight() > 0) {
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

        if (WindowType == Type::SidebarNoMaxHeight && (!Content->Visible || Content->GetHeight() < Height - 15 - 4)) {
            // If the content is smaller than the window, then fill the rest
            // with background
            // Note: this will only look good if the content uses the same background as the window
            canvas.DrawTiled(icons.ClientBackground,
                             offset.X + 4,
                             offset.Y + 15 + (Content->Visible ? Content->GetHeight() : 0),
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
    RenderScrollbar(canvas, offset);

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

Widget::MouseEventResult Window::OnMouseEvent(MouseEvent event, Position position) {
    Widget::MouseEventResult scrollbarResult =
            OnScrollbarMouseEvent(event, position);
    if (scrollbarResult != Widget::MouseEventResult::NotHandled) {
        return scrollbarResult;
    }

    if (PointInsideWidget(position - MinimizeButtonPosition, MinimizeButton)) {
        return MinimizeButton.OnMouseEvent(event, position - MinimizeButtonPosition);
    }

    if (PointInsideWidget(position - CloseButtonPosition, CloseButton)) {
        return CloseButton.OnMouseEvent(event, position - CloseButtonPosition);
    }

    if (event == MouseEvent::LeftDown) {
        // If click is on the header, then start drag action
        if (position.Y < 15) {
            return Widget::MouseEventResult::StartDrag;
        }

        // If click is on the bottom, then start resize action
        if (!MinimizeButton.IsToggled() && position.Y >= Height - 4) {
            return Widget::MouseEventResult::Resize;
        }
    }

    return Widget::MouseEventResult::NotHandled;
}

void Window::MinimizeOnClick() {
    CancelScrollbarDrag();

    if (MinimizeButton.IsToggled()) {
        MaximizedHeight = Height;
        Height = 19;
    } else {
        Height = MaximizedHeight;
    }

    ClampScrollOffset();
}

} // namespace gui
} // namespace trc
