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

#include "builder_sidebar.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <utility>

#include "state.hpp"

#include "gui/border.hpp"
#include "gui/button.hpp"
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
    bool MinimizeButtonPressed = false;

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
        MinimizeButtonPressed = false;
        if (position.X >= 8 && position.X < 8 + MinimizeButton.Width &&
            position.Y >= 4 && position.Y < 4 + MinimizeButton.Height) {
            MinimizeButton.MouseLeftDown(position - gui::Position(8, 4));
            MinimizeButtonPressed = true;
            return MouseEventResult::Handled;
        }
        return MouseEventResult::StartDrag;
    }

    void MouseLeftUp(gui::Position position) override {
        if (MinimizeButtonPressed) {
            MinimizeButtonPressed = false;
            MinimizeButton.MouseLeftUp(position - gui::Position(8, 4));
        }
    }
};

struct SidebarButtons : public gui::Widget {
    Gamestate *_Gamestate;
    GuiState *_GuiState;

    gui::PlacedWidget<gui::Button> SkillsButton;
    gui::PlacedWidget<gui::Button> BattleButton;
    gui::PlacedWidget<gui::Button> VIPButton;

    gui::PlacedWidget<gui::Button> *WidgetPressed;

    SidebarButtons(Gamestate *gamestate, GuiState *guiState)
        : Widget(172, 26), _Gamestate(gamestate), _GuiState(guiState), WidgetPressed(nullptr) {
        auto skillsButton = std::make_unique<gui::Button>(
                &gamestate->Version.Icons.Button34px,
                &gamestate->Version.Icons.Button34pxPressed,
                gui::Button::ButtonType::Toggle);
        skillsButton->SetText("Skills",
                              Pixel(0xFF, 0xFF, 0xFF),
                              &gamestate->Version.Fonts.InterfaceSmall);
        skillsButton->SetOnClick([this]() {
            _GuiState->SkillsWindowVisible = !_GuiState->SkillsWindowVisible;
        });
        skillsButton->Toggled = _GuiState->SkillsWindowVisible;
        SkillsButton.Widget = std::move(skillsButton);
        SkillsButton.Position = gui::Position(8, 3);

        auto battleButton = std::make_unique<gui::Button>(
                &gamestate->Version.Icons.Button34px,
                &gamestate->Version.Icons.Button34pxPressed,
                gui::Button::ButtonType::Toggle);
        battleButton->SetText("Battle",
                              Pixel(0xFF, 0xFF, 0xFF),
                              &gamestate->Version.Fonts.InterfaceSmall);
        battleButton->SetOnClick([this]() {
            _GuiState->BattleWindowVisible = !_GuiState->BattleWindowVisible;
        });
        BattleButton.Widget = std::move(battleButton);
        BattleButton.Position = gui::Position(45, 3);

        auto vipButton = std::make_unique<gui::Button>(
                &gamestate->Version.Icons.Button34px,
                &gamestate->Version.Icons.Button34pxPressed,
                gui::Button::ButtonType::Toggle);
        vipButton->SetText("VIP",
                           Pixel(0xFF, 0xFF, 0xFF),
                           &gamestate->Version.Fonts.InterfaceSmall);
        vipButton->SetOnClick([this]() {
            _GuiState->VIPWindowVisible = !_GuiState->VIPWindowVisible;
        });
        VIPButton.Widget = std::move(vipButton);
        VIPButton.Position = gui::Position(82, 3);
    }

    void Update(gui::State &state, gui::Position offset) override {
        SkillsButton.Widget->Toggled = _GuiState->SkillsWindowVisible;
        SkillsButton.Widget->Update(state, offset + SkillsButton.Position);

        BattleButton.Widget->Toggled = _GuiState->BattleWindowVisible;
        BattleButton.Widget->Update(state, offset + BattleButton.Position);

        VIPButton.Widget->Toggled = _GuiState->VIPWindowVisible;
        VIPButton.Widget->Update(state, offset + VIPButton.Position);
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;
        const auto &fonts = _Gamestate->Version.Fonts;

        canvas.DrawTiled(icons.ClientBackground,
                         offset.X,
                         offset.Y,
                         offset.X + 172,
                         offset.Y + 26);

        SkillsButton.Widget->Render(canvas, offset + SkillsButton.Position);
        BattleButton.Widget->Render(canvas, offset + BattleButton.Position);
        VIPButton.Widget->Render(canvas, offset + VIPButton.Position);

        // Logout button is non-functional
        canvas.Draw(icons.Button43px, offset.X + 124, offset.Y + 3);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 145,
                                         offset.Y + 9,
                                         "Logout",
                                         canvas);
    }

    MouseEventResult MouseLeftDown(gui::Position position) override {
        for (auto *widget : {&SkillsButton, &BattleButton, &VIPButton}) {
            if (position.X >= widget->Position.X &&
                position.X < widget->Position.X + widget->Widget->Width &&
                position.Y >= widget->Position.Y &&
                position.Y < widget->Position.Y + widget->Widget->Height) {
                if (widget->Widget->MouseLeftDown(position -
                                                  widget->Position) ==
                    gui::Widget::MouseEventResult::Handled) {
                    WidgetPressed = widget;
                    return gui::Widget::MouseEventResult::Handled;
                }
            }
        }
        return MouseEventResult::StartDrag;
    }

    void MouseLeftUp(gui::Position position) override {
        if (WidgetPressed)
            WidgetPressed->Widget->MouseLeftUp(position -
                                               WidgetPressed->Position);
        WidgetPressed = nullptr;
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

struct SidebarBattleContent : public gui::Widget {

    Gamestate *_Gamestate;

    SidebarBattleContent(Gamestate *gamestate) : Widget(172 - 8, 0), _Gamestate(gamestate) {
    }

    void Update(gui::State &state, gui::Position offset) override {
        Height = 10 + (_Gamestate->Creatures.size() * 14);
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;

        canvas.DrawTiled(icons.ClientBackground,
                         offset.X,
                         offset.Y,
                         offset.X + Width,
                         offset.Y + Height);

        // TODO: Add healthbar, creature look, etc and fix positions/offsets
        int y = offset.Y + 5;
        for (const auto &[id, creature] : _Gamestate->Creatures) {
            TextRenderer::DrawString(_Gamestate->Version.Fonts.InterfaceLarge,
                                     Pixel(0xBF, 0xBF, 0xBF),
                                     offset.X + 10,
                                     y,
                                     creature.Name,
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

struct SidebarBottom : public gui::VerticalPanel {
    GuiState *_GuiState;
    gui::Window *SkillsWindow;
    gui::Window *BattleWindow;

    SidebarBottom(Gamestate *gamestate, GuiState *guiState)
        : VerticalPanel(176, 0), _GuiState(guiState) {
        StretchLastChild = true;

        // Windows
        SkillsWindow = &Add(std::make_unique<gui::Window>(
                176,
                100,
                &gamestate->Version,
                gui::Window::Type::Sidebar,
                &gamestate->Version.Icons.SkillsIcon,
                "Skills",
                [this]() { _GuiState->SkillsWindowVisible = false; }));
        SkillsWindow->Content =
                std::make_unique<SidebarSkillsContent>(gamestate);

        BattleWindow = &Add(std::make_unique<gui::Window>(
                176,
                100,
                &gamestate->Version,
                gui::Window::Type::SidebarNoMaxHeight,
                &gamestate->Version.Icons.BattleIcon,
                "Battle",
                [this]() { _GuiState->BattleWindowVisible = false; }));
        BattleWindow->Content =
                std::make_unique<SidebarBattleContent>(gamestate);

        // Containers ...

        // Bottom (filler)
        Add(std::make_unique<gui::Border>(
                &gamestate->Version.Icons,
                gui::Border::BorderType::Raised,
                std::make_unique<SidebarBottomFiller>(gamestate)));
    }

    void Update(gui::State &state, gui::Position offset) override {
        // TODO: when a window goes from hidden to visible it should be brought to the bottom of the visible windows
        SkillsWindow->Visible = _GuiState->SkillsWindowVisible;
        BattleWindow->Visible = _GuiState->BattleWindowVisible;
        VerticalPanel::Update(state, offset);
    }
};

std::unique_ptr<gui::Widget> Builder::BuildSidebar(int windowHeight,
                                                   trc::Gamestate *gamestate,
                                                   GuiState *guiState) {
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
    auto sidebar = std::make_unique<gui::VerticalPanel>(176, windowHeight);
    sidebar->StretchLastChild = true;

    // Sidebar top
    // Dynamic size based on content (SidebarInventory can be
    // minimized/maximized)
    auto sidebarTop = std::make_unique<gui::VerticalPanel>(172, 0);
    sidebarTop->DynamicHeight = true;
    sidebarTop->Add(std::make_unique<SidebarMinimap>(gamestate));
    sidebarTop->Add(std::make_unique<SidebarResources>(gamestate));
    sidebarTop->Add(std::make_unique<SidebarInventory>(gamestate));
    sidebarTop->Add(std::make_unique<SidebarButtons>(gamestate, guiState));
    sidebar->Add(std::make_unique<gui::Border>(&gamestate->Version.Icons,
                                               gui::Border::BorderType::Raised,
                                               std::move(sidebarTop)));

    // Sidebar bottom
    // Dynamic size based on parent (Sidebar), it should fill the remaining
    // space And what should fill the remaining space is the bottom/last widget
    // in sidebarBottom
    sidebar->Add(std::make_unique<SidebarBottom>(gamestate, guiState));

    return sidebar;
}
