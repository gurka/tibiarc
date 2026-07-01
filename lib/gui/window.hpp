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

#ifndef __TRC_GUI_WINDOW_HPP__
#define __TRC_GUI_WINDOW_HPP__

#include <functional>
#include <string>
#include <memory>

#include "gui/button.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"
#include "gui/widget.hpp"

namespace trc {

class Canvas;
struct Sprite;
struct Version;

namespace gui {

struct Window : public Widget {
    std::unique_ptr<Widget> Content;

    using OnClickHandler = std::function<void()>;

    enum class Type { Sidebar };

    Window(int width,
           int height,
           const Version *version,
           Type type,
           const Sprite *icon,
           const std::string &title,
           const OnClickHandler &closeOnClick);

    void Update(State &state, Position offset) override;
    void Render(Canvas &canvas, Position offset) override;

    MouseEventResult MouseLeftDown(Position position) override;
    void MouseLeftUp(Position position) override;

private:
    const Version *_Version;
    Type WindowType;
    const Sprite *Icon;
    std::string Title;
    OnClickHandler CloseOnClick;

    // Note: Content should be 8x19 pixels smaller than the window, to perfectly fit
    std::unique_ptr<Canvas> ContentCanvas;
    int ScrollOffset;

    int MaximizedHeight;
    Button MinimizeButton;
    Position MinimizeButtonPosition;
    Button CloseButton;
    Position CloseButtonPosition;

    Button *PressedButton = nullptr;

    void MinimizeOnClick();
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_WINDOW_HPP__

