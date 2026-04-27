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

#include "button.hpp"
#include "canvas.hpp"
#include "sprites.hpp"
#include "state.hpp"
#include "versions.hpp"
#include "widget.hpp"

namespace trc {
namespace gui {

struct Window : public Widget {
    const Version *_Version;
    enum class Type { Sidebar } WindowType;
    const Sprite *Icon;
    std::string Title;

    bool Minimized = false;
    Button MinimizeButton;
    Button CloseButton;

    Window(int x,
           int y,
           int width,
           int height,
           const Version *version,
           Type type,
           const Sprite *icon,
           const std::string &title);

    void Render(Canvas &canvas, const State &state) override;

    void MouseLeftDown(int x, int y) override;
    void MouseLeftUp(int x, int y) override;
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_WINDOW_HPP__

