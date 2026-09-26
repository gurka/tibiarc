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

#include "gui/scrollable_widget.hpp"

#include <algorithm>

#include "gui/common.hpp"
#include "gui/state.hpp"

#include "canvas.hpp"
#include "versions.hpp"

namespace trc {
namespace gui {

ScrollableWidget::ScrollableWidget(int width, int height, const Version *version)
    : Widget(width, height),
      _Version(version),
      ScrollUpButton(&version->Icons.ScrollbarUp,
                     &version->Icons.ScrollbarUpPressed),
      ScrollDownButton(&version->Icons.ScrollbarDown,
                       &version->Icons.ScrollbarDownPressed) {
    ScrollUpButton.SetOnClick([this]() {
        ScrollOffset -= 10;
        ClampScrollOffset();
    });
    ScrollDownButton.SetOnClick([this]() {
        ScrollOffset += 10;
        ClampScrollOffset();
    });
}

int ScrollableWidget::MaxScrollOffset() const {
    return std::max(0, GetScrollContentHeight() - GetScrollViewportHeight());
}

void ScrollableWidget::ClampScrollOffset() {
    ScrollOffset = std::clamp(ScrollOffset, 0, MaxScrollOffset());
}

bool ScrollableWidget::CanScroll() const {
    return GetScrollViewportHeight() > 0 && MaxScrollOffset() > 0;
}

void ScrollableWidget::CancelScrollbarDrag() {
    ScrollbarThumbPressed = false;
}

bool ScrollableWidget::GetScrollbarThumbMetrics(int &thumbOffset,
                                                int &thumbHeight) const {
    const int scrollbarHeight = std::max(0, GetScrollbarTrackHeight());
    if (scrollbarHeight <= 0 || !CanScroll()) {
        return false;
    }

    thumbHeight = std::clamp(
            (scrollbarHeight * GetScrollViewportHeight()) / GetScrollContentHeight(),
            _Version->Icons.ScrollbarThumb.Height,
            scrollbarHeight);

    thumbOffset = 0;
    const int scrollbarThumbRange = scrollbarHeight - thumbHeight;
    const int maxScrollOffset = MaxScrollOffset();
    if (scrollbarThumbRange > 0 && maxScrollOffset > 0) {
        thumbOffset = (ScrollOffset * scrollbarThumbRange) / maxScrollOffset;
    }

    return true;
}

void ScrollableWidget::UpdateScrollbar(State &state, Position offset) {
    ClampScrollOffset();

    if (ScrollbarThumbPressed) {
        if (!state.MouseLeftDown()) {
            ScrollbarThumbPressed = false;
        } else {
            int scrollbarThumbOffset = 0;
            int scrollbarThumbHeight = 0;
            if (GetScrollbarThumbMetrics(scrollbarThumbOffset,
                                         scrollbarThumbHeight)) {
                const int trackTop = GetScrollbarTrackTop();
                const int scrollbarHeight = GetScrollbarTrackHeight();
                const int scrollbarThumbRange = scrollbarHeight - scrollbarThumbHeight;
                if (scrollbarThumbRange > 0) {
                    const int mouseY = state.MousePosition(offset).Y;
                    const int scrollbarThumbTop =
                            std::clamp(mouseY - ScrollbarThumbDragOffsetY,
                                       trackTop,
                                       trackTop + scrollbarThumbRange);
                    const int thumbOffset = scrollbarThumbTop - trackTop;
                    ScrollOffset =
                            (thumbOffset * MaxScrollOffset()) / scrollbarThumbRange;
                    ClampScrollOffset();
                }
            } else {
                ScrollbarThumbPressed = false;
            }
        }
    }

    ScrollUpButton.Update(state, offset + GetScrollUpButtonPosition());
    ScrollDownButton.Update(state, offset + GetScrollDownButtonPosition());
}

void ScrollableWidget::RenderScrollbar(Canvas &canvas, Position offset) {
    const auto &icons = _Version->Icons;

    ScrollUpButton.Render(canvas, offset + GetScrollUpButtonPosition());

    const int scrollbarX = offset.X + GetScrollbarX();
    const int scrollbarTrackTop = offset.Y + GetScrollbarTrackTop();
    const int scrollbarTrackHeight = GetScrollbarTrackHeight();

    canvas.DrawTiled(icons.ScrollbarBackground,
                     scrollbarX,
                     scrollbarTrackTop,
                     scrollbarX + 12,
                     scrollbarTrackTop + scrollbarTrackHeight);

    int scrollbarThumbOffset = 0;
    int scrollbarThumbHeight = 0;
    if (GetScrollbarThumbMetrics(scrollbarThumbOffset, scrollbarThumbHeight)) {
        if (scrollbarThumbHeight == icons.ScrollbarThumb.Height) {
            canvas.Draw(icons.ScrollbarThumb,
                        scrollbarX,
                        scrollbarTrackTop + scrollbarThumbOffset);
        } else {
            const int scrollbarThumbTopY = scrollbarTrackTop + scrollbarThumbOffset;
            const int scrollbarThumbMiddleY =
                    scrollbarThumbTopY + icons.ScrollbarThumbTopPart.Height;
            const int scrollbarThumbBottomY =
                    scrollbarThumbTopY + scrollbarThumbHeight -
                    icons.ScrollbarThumbBottomPart.Height;

            canvas.Draw(icons.ScrollbarThumbTopPart,
                        scrollbarX,
                        scrollbarThumbTopY);
            canvas.DrawTiled(icons.ScrollbarThumbMiddlePart,
                             scrollbarX,
                             scrollbarThumbMiddleY,
                             scrollbarX + icons.ScrollbarThumbMiddlePart.Width,
                             scrollbarThumbBottomY);
            canvas.Draw(icons.ScrollbarThumbBottomPart,
                        scrollbarX,
                        scrollbarThumbBottomY);
        }
    }

    ScrollDownButton.Render(canvas, offset + GetScrollDownButtonPosition());
}

Widget::MouseEventResult ScrollableWidget::OnScrollbarMouseEvent(MouseEvent event,
                                                                 Position position) {
    if (event == MouseEvent::LeftDown) {
        PressedScrollButton = nullptr;
        ScrollbarThumbPressed = false;

        const Position scrollUpPos = GetScrollUpButtonPosition();
        if (PointInsideWidget(position - scrollUpPos, ScrollUpButton)) {
            PressedScrollButton = &ScrollUpButton;
            return ScrollUpButton.OnMouseEvent(event, position - scrollUpPos);
        }

        const Position scrollDownPos = GetScrollDownButtonPosition();
        if (PointInsideWidget(position - scrollDownPos, ScrollDownButton)) {
            PressedScrollButton = &ScrollDownButton;
            return ScrollDownButton.OnMouseEvent(event, position - scrollDownPos);
        }

        int scrollbarThumbOffset = 0;
        int scrollbarThumbHeight = 0;
        const int scrollbarX = GetScrollbarX();
        const int trackTop = GetScrollbarTrackTop();
        if (GetScrollbarThumbMetrics(scrollbarThumbOffset, scrollbarThumbHeight) &&
            position.X >= scrollbarX && position.X < scrollbarX + 12 &&
            position.Y >= trackTop + scrollbarThumbOffset &&
            position.Y < trackTop + scrollbarThumbOffset + scrollbarThumbHeight) {
            ScrollbarThumbPressed = true;
            ScrollbarThumbDragOffsetY =
                    position.Y - (trackTop + scrollbarThumbOffset);
            return Widget::MouseEventResult::Handled;
        }

        return Widget::MouseEventResult::NotHandled;
    } else if (event == MouseEvent::LeftUp) {
        if (PressedScrollButton != nullptr) {
            const Position pos = PressedScrollButton == &ScrollUpButton
                                          ? GetScrollUpButtonPosition()
                                          : GetScrollDownButtonPosition();
            PressedScrollButton->OnMouseEvent(event, position - pos);
            PressedScrollButton = nullptr;
        }

        if (ScrollbarThumbPressed) {
            ScrollbarThumbPressed = false;
            return Widget::MouseEventResult::Handled;
        }

        return Widget::MouseEventResult::NotHandled;
    } else if (event == MouseEvent::WheelUp) {
        if (!CanScroll()) {
            return Widget::MouseEventResult::NotHandled;
        }
        ScrollOffset -= 10;
        ClampScrollOffset();
        return Widget::MouseEventResult::Handled;
    } else {
        // WheelDown
        if (!CanScroll()) {
            return Widget::MouseEventResult::NotHandled;
        }
        ScrollOffset += 10;
        ClampScrollOffset();
        return Widget::MouseEventResult::Handled;
    }
}

} // namespace gui
} // namespace trc
