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

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <utility>

#include "gui/border.hpp"
#include "gui/button.hpp"
#include "gui/panel.hpp"
#include "gui/position.hpp"
#include "gui/vertical_panel.hpp"
#include "gui/widget.hpp"
#include "gui/window.hpp"
#include "gui/state.hpp"

#include "canvas.hpp"
#include "gamestate.hpp"
#include "pixel.hpp"
#include "player.hpp"
#include "renderer.hpp"
#include "textrenderer.hpp"
#include "versions.hpp"

using namespace trc;

struct SidebarMinimap : public gui::Widget {
    Gamestate *_Gamestate;

    SidebarMinimap(Gamestate *gamestate)
        : Widget(172, 117), _Gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &fonts = _Gamestate->Version.Fonts;
        const auto &icons = _Gamestate->Version.Icons;

        canvas.DrawTiled(icons.ClientBackground,
                         offset.X,
                         offset.Y,
                         offset.X + 172,
                         offset.Y + 117);

        // Minimap, empty for now
        // TODO: use Border widget
        /*
        Common::DrawBorder1px(icons,
                              canvas,
                              offset.X + 8,
                              offset.Y + 4,
                              offset.X + 8 + 108,
                              offset.Y + 4 + 108);
        */
        canvas.DrawRectangle(Pixel(0, 0, 0),
                             offset.X + 9,
                             offset.Y + 5,
                             106,
                             106);

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

    SidebarResources(Gamestate *gamestate)
        : Widget(172, 32), _Gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;
        const auto &fonts = _Gamestate->Version.Fonts;

        canvas.DrawTiled(icons.ClientBackground,
                         offset.X,
                         offset.Y,
                         offset.X + 172,
                         offset.Y + 32);

        // Health
        canvas.Draw(icons.HealthIcon, offset.X + 9, offset.Y + 5);
        canvas.Draw(icons.EmptyStatusBar, offset.X + 26, offset.Y + 4);
        if (_Gamestate->Player.Stats.MaxHealth > 0 &&
            _Gamestate->Player.Stats.Health <=
                    _Gamestate->Player.Stats.MaxHealth) {
            canvas.Draw(
                    icons.HealthBar,
                    offset.X + 26,
                    offset.Y + 4,
                    (icons.HealthBar.Width * _Gamestate->Player.Stats.Health) /
                            _Gamestate->Player.Stats.MaxHealth,
                    11);
        }
        TextRenderer::DrawString(
                fonts.InterfaceLarge,
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

struct SidebarInventory : public gui::Widget {
    Gamestate *_Gamestate;
    gui::Button MinimizeButton;

    SidebarInventory(Gamestate *gamestate)
        : Widget(172, 155),
          _Gamestate(gamestate),
          MinimizeButton(&gamestate->Version.Icons.Minimize,
                         &gamestate->Version.Icons.MinimizePressed,
                         &gamestate->Version.Icons.Maximize,
                         &gamestate->Version.Icons.MaximizePressed) {
        MinimizeButton.SetOnClick(
                [this]() { Height = MinimizeButton.Toggled ? 48 : 155; });
    }

    void Update(gui::State &state, gui::Position offset) override {
        MinimizeButton.Update(state, offset + gui::Position(8, 4));
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;
        const auto &fonts = _Gamestate->Version.Fonts;

        if (!MinimizeButton.Toggled) {
            canvas.DrawTiled(icons.ClientBackground,
                             offset.X,
                             offset.Y,
                             offset.X + 172,
                             offset.Y + 155);

            // Inventory
            MinimizeButton.Render(canvas, offset + gui::Position(8, 4));

            for (const auto &[slot, x, y] :
                 std::initializer_list<std::tuple<InventorySlot, int, int>>{
                         {InventorySlot::Head, offset.X + 45, offset.Y + 4},
                         {InventorySlot::Amulet, offset.X + 8, offset.Y + 18},
                         {InventorySlot::Backpack,
                          offset.X + 82,
                          offset.Y + 18},
                         {InventorySlot::Chest, offset.X + 45, offset.Y + 41},
                         {InventorySlot::RightArm, offset.X + 8, offset.Y + 55},
                         {InventorySlot::LeftArm, offset.X + 82, offset.Y + 55},
                         {InventorySlot::Legs, offset.X + 45, offset.Y + 78},
                         {InventorySlot::Ring, offset.X + 8, offset.Y + 92},
                         {InventorySlot::Quiver, offset.X + 82, offset.Y + 92},
                         {InventorySlot::Boots, offset.X + 45, offset.Y + 115},
                 }) {
                Renderer::DrawInventorySlot(*_Gamestate, slot, x, y, canvas);
            }

            // TODO
            /*
            if (!version.Features.IconBar) {
                DrawIconArea(gamestate, canvas, 10, 279);
            }
            if (version.Features.IconBar) {
                canvas.Draw(icons.SecondaryStatBackground, 10, 279);
                TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                                 Pixel(0xFF, 0xFF, 0xFF),
                                                 16 + baseX + 17,
                                                 baseY + 2,
                                                 "Soul:",
                                                 canvas);

                TextRenderer::DrawCenteredString(
                        version.Fonts.InterfaceLarge,
                        Pixel(0xAF, 0xAF, 0xAF),
                        16 + baseX + 17,
                        baseY + 10,
                        Format("{}", gamestate.Player.Stats.SoulPoints),
                        canvas);
            }
            */

            canvas.Draw(icons.SecondaryStatBackground,
                        offset.X + 82,
                        offset.Y + 128);
            TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                             Pixel(0xFF, 0xFF, 0xFF),
                                             offset.X + 99,
                                             offset.Y + 130,
                                             "Cap:",
                                             canvas);
            uint32_t capacity = _Gamestate->Player.Stats.Capacity /
                                _Gamestate->Version.Features.CapacityDivisor;
            TextRenderer::DrawCenteredString(fonts.InterfaceLarge,
                                             Pixel(0xBF, 0xBF, 0xBF),
                                             offset.X + 99,
                                             offset.Y + 139,
                                             std::to_string(capacity),
                                             canvas);

            // Attack mode buttons
            canvas.Draw(icons.FightingOffensive, offset.X + 124, offset.Y + 19);
            canvas.Draw(icons.FightingBalanced, offset.X + 124, offset.Y + 39);
            canvas.Draw(icons.FightingDefensive, offset.X + 124, offset.Y + 59);
            canvas.Draw(icons.AttackStanding, offset.X + 147, offset.Y + 19);
            canvas.Draw(icons.AttackChasing, offset.X + 147, offset.Y + 39);
            canvas.Draw(icons.AttackUnmarked, offset.X + 147, offset.Y + 59);

            // Buttons
            canvas.Draw(icons.Button43px, offset.X + 124, offset.Y + 83);
            TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                             Pixel(0xFF, 0xFF, 0xFF),
                                             offset.X + 145,
                                             offset.Y + 90,
                                             "Stop",
                                             canvas);

            canvas.Draw(icons.Button43px, offset.X + 124, offset.Y + 107);
            TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                             Pixel(0xFF, 0xFF, 0xFF),
                                             offset.X + 145,
                                             offset.Y + 114,
                                             "Options",
                                             canvas);

            canvas.Draw(icons.Button43px, offset.X + 124, offset.Y + 131);
            TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                             Pixel(0xFF, 0xFF, 0xFF),
                                             offset.X + 145,
                                             offset.Y + 138,
                                             "Help",
                                             canvas);
        } else {
            canvas.DrawTiled(icons.ClientBackground,
                             offset.X,
                             offset.Y,
                             offset.X + 172,
                             offset.Y + 48);

            // Inventory
            MinimizeButton.Render(canvas, offset + gui::Position(8, 4));

            // Status background
            canvas.Draw(icons.MinimizedInventoryStatusBackground,
                        offset.X + 22,
                        offset.Y + 4);
            TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                             Pixel(0xFF, 0xFF, 0xFF),
                                             offset.X + 39,
                                             offset.Y + 6,
                                             "Cap:",
                                             canvas);
            uint32_t capacity = _Gamestate->Player.Stats.Capacity /
                                _Gamestate->Version.Features.CapacityDivisor;
            TextRenderer::DrawCenteredString(
                    _Gamestate->Version.Fonts.InterfaceLarge,
                    Pixel(0xBF, 0xBF, 0xBF),
                    offset.X + 39,
                    offset.Y + 15,
                    std::to_string(capacity),
                    canvas);

            // TODO: Draw status icons

            // Buttons
            canvas.Draw(icons.FightingOffensive, offset.X + 56, offset.Y + 4);
            canvas.Draw(icons.FightingBalanced, offset.X + 76, offset.Y + 4);
            canvas.Draw(icons.FightingDefensive, offset.X + 96, offset.Y + 4);
            canvas.Draw(icons.AttackStanding, offset.X + 56, offset.Y + 26);
            canvas.Draw(icons.AttackChasing, offset.X + 76, offset.Y + 26);
            canvas.Draw(icons.AttackUnmarked, offset.X + 96, offset.Y + 26);
        }
    }

    MouseEventResult MouseLeftDown(gui::Position position) override {
        if (position.X >= 8 && position.X < 8 + MinimizeButton.Width &&
            position.Y >= 4 && position.Y < 4 + MinimizeButton.Height) {
            MinimizeButton.MouseLeftDown(position - gui::Position(8, 4));
            return MouseEventResult::Handled;
        }
        return MouseEventResult::StartDrag;
    }

    void MouseLeftUp(gui::Position position) override {
        if (position.X >= 8 && position.X < 8 + MinimizeButton.Width &&
            position.Y >= 4 && position.Y < 4 + MinimizeButton.Height) {
            MinimizeButton.MouseLeftUp(position - gui::Position(8, 4));
        }
    }
};

struct SidebarButtons : public gui::Widget {
    Gamestate *_Gamestate;

    SidebarButtons(Gamestate *gamestate)
        : Widget(172, 26), _Gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;
        const auto &fonts = _Gamestate->Version.Fonts;

        canvas.DrawTiled(icons.ClientBackground,
                         offset.X,
                         offset.Y,
                         offset.X + 172,
                         offset.Y + 26);

        canvas.Draw(icons.Button34px, offset.X + 8, offset.Y + 3);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 25,
                                         offset.Y + 9,
                                         "Skills",
                                         canvas);

        canvas.Draw(icons.Button34px, offset.X + 45, offset.Y + 3);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 62,
                                         offset.Y + 9,
                                         "Battle",
                                         canvas);

        canvas.Draw(icons.Button34px, offset.X + 82, offset.Y + 3);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 99,
                                         offset.Y + 9,
                                         "VIP",
                                         canvas);

        canvas.Draw(icons.Button43px, offset.X + 124, offset.Y + 3);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 145,
                                         offset.Y + 9,
                                         "Logout",
                                         canvas);
    }

    MouseEventResult MouseLeftDown(gui::Position position) override {
        return MouseEventResult::StartDrag;
    }
};

struct SidebarSkillsContent : public gui::Widget {
    Gamestate *_Gamestate;

    SidebarSkillsContent(Gamestate *gamestate)
        : Widget(172, 14 * 15), _Gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;
        const auto &fonts = _Gamestate->Version.Fonts;

        canvas.DrawTiled(icons.ClientBackground,
                         offset.X,
                         offset.Y,
                         offset.X + Width,
                         offset.Y + Height);

        const auto stats = std::vector<
                std::tuple<std::string,
                           std::function<uint64_t(const PlayerData &)>>>{
                {"Experience",
                 [](const PlayerData &player) {
                     return player.Stats.Experience;
                 }},
                {"Level",
                 [](const PlayerData &player) { return player.Stats.Level; }},
                {"Hit points",
                 [](const PlayerData &player) { return player.Stats.Health; }},
                {"Mana",
                 [](const PlayerData &player) { return player.Stats.Mana; }},
                {"Speed",
                 [](const PlayerData &player) { return player.Stats.Speed; }},
                {"Capacity",
                 [](const PlayerData &player) {
                     return player.Stats.Capacity;
                 }},
                {"Magic Level",
                 [](const PlayerData &player) {
                     return player.Stats.MagicLevel;
                 }},
        };
        const auto skills = std::vector<std::tuple<std::string, int>>{
                {"Fist Fighting", 0},
                {"Club Fighting", 1},
                {"Sword Fighting", 2},
                {"Axe Fighting", 3},
                {"Distance Fighting", 4},
                {"Shielding", 5},
                {"Fishing", 6},
        };

        int y = offset.Y + 10;
        for (const auto &[stat, statFunc] : stats) {
            TextRenderer::DrawString(fonts.InterfaceLarge,
                                     Pixel(0xAF, 0xAF, 0xAF),
                                     offset.X + 10,
                                     y,
                                     stat,
                                     canvas);
            TextRenderer::DrawRightAlignedString(
                    fonts.InterfaceLarge,
                    Pixel(0xAF, 0xAF, 0xAF),
                    offset.X + 145,
                    y,
                    ThousandSeparators(statFunc(_Gamestate->Player)),
                    canvas);

            y += 14;
        }
        for (const auto &[skill, skillIndex] : skills) {
            TextRenderer::DrawString(fonts.InterfaceLarge,
                                     Pixel(0xAF, 0xAF, 0xAF),
                                     offset.X + 10,
                                     y,
                                     skill,
                                     canvas);
            TextRenderer::DrawRightAlignedString(
                    fonts.InterfaceLarge,
                    Pixel(0xAF, 0xAF, 0xAF),
                    offset.X + 145,
                    y,
                    std::to_string(
                            _Gamestate->Player.Skills[skillIndex].Effective),
                    canvas);
            y += 14;
        }
    }
};

struct SidebarBottomFiller : public gui::Widget {

    Gamestate *_Gamestate;

    SidebarBottomFiller(Gamestate *gamestate)
        : Widget(172, 0), _Gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;

        canvas.DrawTiledBottomUp(icons.ClientBackground,
                                 offset.X,
                                 offset.Y,
                                 offset.X + Width,
                                 offset.Y + Height);
    }
};

std::unique_ptr<gui::Panel> Builder::BuildGui(int width,
                                              int height,
                                              trc::Gamestate *gamestate) {
    auto gui = std::make_unique<gui::Panel>(width, height);

    // The sidebar is built as:
    // - VerticalPanel that contains:
    //   - Border that contains:
    //     - VerticalPanel (SidebarTop) that contains:
    //       - SidebarMinimap
    //       - SidebarResources
    //       - SidebarInventory
    //       - SidebarButtons
    //   - VerticalPanel (SidebarBottom) that contains:
    //     - SidebarSkills
    //     - SidebarBattle
    //     - SidebarVIP
    //     - Open container 1..n
    //     - SidebarEmpty (remaining space, can be invisible/Height=0)

    // Sidebar
    // Always the same height (window/gui height)
    auto sidebar = std::make_unique<gui::VerticalPanel>(176, height);
    sidebar->ResizeBottomWidget = true;

    // Sidebar top
    // Dynamic size based on content (SidebarInventory can be
    // minimized/maximized)
    auto sidebarTop = std::make_unique<gui::VerticalPanel>(172, 0);
    sidebarTop->DynamicHeight = true;
    sidebarTop->Widgets.emplace_back(
            std::make_unique<SidebarMinimap>(gamestate));
    sidebarTop->Widgets.emplace_back(
            std::make_unique<SidebarResources>(gamestate));
    sidebarTop->Widgets.emplace_back(
            std::make_unique<SidebarInventory>(gamestate));
    sidebarTop->Widgets.emplace_back(
            std::make_unique<SidebarButtons>(gamestate));
    sidebar->Widgets.emplace_back(
            std::make_unique<gui::Border>(&gamestate->Version.Icons,
                                          gui::Border::BorderType::Raised,
                                          std::move(sidebarTop)));

    // Sidebar bottom
    // Dynamic size based on parent (Sidebar), it should fill the remaining
    // space And what should fill the remaining space is the bottom/last widget
    // in sidebarBottom
    auto sidebarBottom = std::make_unique<gui::VerticalPanel>(176, 0);
    sidebarBottom->ResizeBottomWidget = true;
    auto sidebarSkillsWindow =
            std::make_unique<gui::Window>(176,
                                          100,
                                          &gamestate->Version,
                                          gui::Window::Type::Sidebar,
                                          &gamestate->Version.Icons.SkillsIcon,
                                          "Skills",
                                          []() { /* TODO */ });
    sidebarSkillsWindow->Content =
            std::make_unique<SidebarSkillsContent>(gamestate);
    sidebarBottom->Widgets.emplace_back(std::move(sidebarSkillsWindow));
    sidebarBottom->Widgets.emplace_back(std::make_unique<gui::Border>(
            &gamestate->Version.Icons,
            gui::Border::BorderType::Raised,
            std::make_unique<SidebarBottomFiller>(gamestate)));
    sidebar->Widgets.emplace_back(std::move(sidebarBottom));

    // Add sidebar to gui
    gui->Widgets.push_back({std::move(sidebar), gui::Position(width - 176, 0)});
    return gui;
}
