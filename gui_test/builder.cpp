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

#include "builder.hpp"

#include <memory>
#include <string>

#include "gui/panel.hpp"
#include "gui/position.hpp"
#include "gui/widget.hpp"
#include "gui/window.hpp"

#include "canvas.hpp"
#include "pixel.hpp"
#include "textrenderer.hpp"
#include "gamestate.hpp"
#include "versions.hpp"

#include "common.hpp"

using namespace trc;

struct SidebarMinimap : public gui::Widget {
    Gamestate *_Gamestate;

    SidebarMinimap(Gamestate *gamestate) : Widget(172, 117), _Gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &fonts = _Gamestate->Version.Fonts;
        const auto &icons = _Gamestate->Version.Icons;

        canvas.DrawBackground(icons.ClientBackground,
                              offset.X,
                              offset.Y,
                              offset.X + 172,
                              offset.Y + 117);

        // Minimap, empty for now
        Common::DrawBorder1px(icons,
                              canvas,
                              offset.X + 8,
                              offset.Y + 4,
                              offset.X + 8 + 108,
                              offset.Y + 4 + 108);
        canvas.DrawRectangle(Pixel(0, 0, 0), offset.X + 11, offset.Y + 5, 106, 106);

        // Buttons, since they are disabled we just render them instead
        // of using the Button widget
        canvas.Draw(icons.Compass, offset.X + 124, offset.Y + 5);
        canvas.Draw(icons.ZoomOut, offset.X + 124, offset.Y + 50);
        canvas.Draw(icons.LevelUp, offset.X + 147, offset.Y + 50);
        canvas.Draw(icons.ZoomIn, offset.X + 124, offset.Y + 71);
        canvas.Draw(icons.LevelDown, offset.X + 147, offset.Y + 71);
        canvas.Draw(icons.Button43px, offset.X + 124, offset.Y + 92);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 145,
                                         offset.Y + 98,
                                         "Centre",
                                         canvas);
    }

    MouseEventResult MouseLeftDown(gui::Position position) override {
        return MouseEventResult::StartDrag;
    }
};

struct SidebarResources : public gui::Widget {
    Gamestate *_Gamestate;

    SidebarResources(Gamestate *gamestate) : Widget(172, 32), _Gamestate(gamestate  ) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;
        const auto &fonts = _Gamestate->Version.Fonts;

        canvas.DrawBackground(icons.ClientBackground,
                              offset.X,
                              offset.Y,
                              offset.X + 172,
                              offset.Y + 32);

        // Health
        canvas.Draw(icons.HealthIcon, offset.X + 9, offset.Y + 5);
        canvas.Draw(icons.EmptyStatusBar, offset.X + 26, offset.Y + 4);
        if (_Gamestate->Player.Stats.MaxHealth > 0 &&
            _Gamestate->Player.Stats.Health <= _Gamestate->Player.Stats.MaxHealth) {
            canvas.Draw(
                    icons.HealthBar,
                    offset.X + 26,
                    offset.Y + 4,
                    (icons.HealthBar.Width * _Gamestate->Player.Stats.Health) /
                            _Gamestate->Player.Stats.MaxHealth,
                    11);
        }
        TextRenderer::DrawString(fonts.InterfaceLarge,
                                 Pixel(0xAF, 0xAF, 0xAF),
                                 offset.X + 127,
                                 offset.Y + 5,
                                 std::to_string(_Gamestate->Player.Stats.Health),
                                 canvas);

        // Mana
        canvas.Draw(icons.ManaIcon, offset.X + 9, offset.Y + 18);
        canvas.Draw(icons.EmptyStatusBar, offset.X + 26, offset.Y + 17);
        if (_Gamestate->Player.Stats.MaxMana > 0 &&
            _Gamestate->Player.Stats.Mana <= _Gamestate->Player.Stats.MaxMana) {
            canvas.Draw(icons.ManaBar,
                        offset.X + 26,
                        offset.Y + 17,
                        (icons.ManaBar.Width * _Gamestate->Player.Stats.Mana) /
                                _Gamestate->Player.Stats.MaxMana,
                        11);
        }
        TextRenderer::DrawString(fonts.InterfaceLarge,
                                 Pixel(0xAF, 0xAF, 0xAF),
                                 offset.X + 127,
                                 offset.Y + 17,
                                 std::to_string(_Gamestate->Player.Stats.Mana),
                                 canvas);
    }

    MouseEventResult MouseLeftDown(gui::Position position) override {
        return MouseEventResult::StartDrag;
    }
};

struct SidebarTop : public gui::Panel {
    Gamestate *_Gamestate;

    SidebarTop(int height, Gamestate *gamestate)
        : gui::Panel(176, height, 2), _Gamestate(gamestate) {
        Widgets.emplace_back(std::make_unique<SidebarMinimap>(gamestate),
                             gui::Position(2, 2));
        Widgets.emplace_back(std::make_unique<SidebarResources>(gamestate),
                             gui::Position(2, 119));
    }

    void Update(gui::State &state, gui::Position offset) override {
        gui::Panel::Update(state, offset);
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        Common::DrawBorder2px(_Gamestate->Version.Icons,
                              canvas,
                              offset.X,
                              offset.Y,
                              offset.X + Width,
                              offset.Y + Height);
        gui::Panel::Render(canvas, offset);
    }

    Panel::MouseEventResult MouseLeftDown(gui::Position position) override {
        return gui::Panel::MouseLeftDown(position);
    }

    void MouseLeftUp(gui::Position position) override {
        gui::Panel::MouseLeftUp(position);
    }
};

std::unique_ptr<gui::Panel> Builder::BuildGui(int width, int height, trc::Gamestate *gamestate) {
    auto gui = std::make_unique<gui::Panel>(width, height, 0);

    gui->Widgets.emplace_back(std::make_unique<SidebarTop>(height, gamestate),
                              gui::Position(width - 176, 0));

    return gui;
}
