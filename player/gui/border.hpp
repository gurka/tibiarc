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

#ifndef __TRC_GUI_BORDER_HPP__
#define __TRC_GUI_BORDER_HPP__

#include <memory>

#include "gui/widget.hpp"
#include "icons.hpp"

namespace trc {
namespace gui {

struct Border : Widget {
    enum class BorderType {
        Sunken, // 1px
        Raised, // 2px
    };

    Border(const Icons *icons, BorderType type, std::unique_ptr<Widget> child)
        : Widget(BorderWidth(type) * 2 + child->GetWidth(),
                 BorderWidth(type) * 2 + child->GetHeight()),
          _Icons(icons),
          Type(type),
          Child(std::move(child)) {
    }

    Widget &GetChild() { return *Child; }
    const Widget &GetChild() const { return *Child; }

    void SetWidth(int w) override;
    void SetHeight(int h) override;
    void Update(State &state, Position offset) override;
    void Render(Canvas &canvas, Position offset) override;

    MouseEventResult OnMouseEvent(MouseEvent event, Position position) override;

private:
    bool ChildPressed = false;

    const Icons *_Icons;
    BorderType Type;
    std::unique_ptr<Widget> Child;

    void RenderSunkenBorder(Canvas &canvas, Position offset);
    void RenderRaisedBorder(Canvas &canvas, Position offset);

    constexpr int BorderWidth() const {
        return BorderWidth(Type);
    }

    static constexpr int BorderWidth(BorderType type) {
        switch (type) {
        case BorderType::Sunken:
            return 1;
        case BorderType::Raised:
            return 2;
        }
        std::unreachable();
    }
};

} // namespace gui
} // namespace trc

#endif // __TRC_GUI_BORDER_HPP__
