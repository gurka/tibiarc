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

#include <algorithm>
#include <memory>
#include <string>

#include "gui/common.hpp"
#include "gui/panel.hpp"
#include "gui/position.hpp"
#include "gui/widget.hpp"
#include "gui/state.hpp"

#include "canvas.hpp"
#include "gamestate.hpp"
#include "pixel.hpp"
#include "renderer.hpp"
#include "textrenderer.hpp"
#include "utils.hpp"
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
        canvas.DrawRectangle(Pixel(0, 0, 0), offset.X + 9, offset.Y + 5, 106, 106);

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

struct SidebarInventory : public gui::Widget {
    Gamestate *_Gamestate;
    //gui::Button MinimizeButton;

    SidebarInventory(Gamestate *gamestate) : Widget(172, 155), _Gamestate(gamestate) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &icons = _Gamestate->Version.Icons;
        const auto &fonts = _Gamestate->Version.Fonts;

        //if (!InventoryMinimized) {
        if (true) {
            canvas.DrawBackground(icons.ClientBackground,
                                  offset.X,
                                  offset.Y,
                                  offset.X + 172,
                                  offset.Y + 155);

            // Inventory
            canvas.Draw(icons.Minimize, offset.X + 8, offset.Y + 4);

            for (auto [slot, x, y] :
                 std::initializer_list<std::tuple<InventorySlot, int, int>>{
                         {InventorySlot::Head, offset.X + 45, offset.Y + 4},
                         {InventorySlot::Amulet, offset.X + 8, offset.Y + 18},
                         {InventorySlot::Backpack, offset.X + 82, offset.Y + 18},
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

            canvas.Draw(icons.SecondaryStatBackground, offset.X + 82, offset.Y + 128);
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
            canvas.DrawBackground(icons.ClientBackground,
                                  offset.X,
                                  offset.Y,
                                  offset.X + 172,
                                  offset.Y + 48);

            // Inventory
            canvas.Draw(icons.Maximize, offset.X + 8, offset.Y + 4);

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
            TextRenderer::DrawCenteredString(_Gamestate->Version.Fonts.InterfaceLarge,
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
        return MouseEventResult::StartDrag;
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

        canvas.DrawBackground(icons.ClientBackground,
                              offset.X,
                              offset.Y,
                              offset.X + 172,
                              offset.Y + 26);

        canvas.Draw(icons.Button34px,
                    offset.X + 8,
                    offset.Y + 3);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 25,
                                         offset.Y + 9,
                                         "Skills",
                                         canvas);

        canvas.Draw(icons.Button34px,
                    offset.X + 45,
                    offset.Y + 3);
        TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                         Pixel(0xFF, 0xFF, 0xFF),
                                         offset.X + 62,
                                         offset.Y + 9,
                                         "Battle",
                                         canvas);

        canvas.Draw(icons.Button34px,
                    offset.X + 82,
                    offset.Y + 3);
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

struct SidebarTop : public gui::Panel {
    Gamestate *_Gamestate;

    // This is used to keep track of the empty space created by dragging
    // a widget, which we use to determine when to shift other widgets
    // up or down during the drag action
    int DragEmptyY = 0;
    int DragEmptyHeight = 0;

    SidebarTop(int height, Gamestate *gamestate)
        : gui::Panel(176, height, 2), _Gamestate(gamestate) {

        Widgets.emplace_back(std::make_unique<SidebarMinimap>(gamestate),
                             gui::Position(2, 2));
        Widgets.emplace_back(std::make_unique<SidebarResources>(gamestate),
                             gui::Position(2, 119));
        Widgets.emplace_back(std::make_unique<SidebarInventory>(gamestate),
                             gui::Position(2, 151));
        Widgets.emplace_back(std::make_unique<SidebarButtons>(gamestate),
                             gui::Position(2, 306));
    }

    void Update(gui::State &state, gui::Position offset) override {
        if (DragTarget != nullptr && !state.MouseLeftDown()) {
            // Drag action ended, make sure that the widget ends up
            // in the empty space
            std::get<1>(*GetWidgetAndPosition(DragTarget)).Y = DragEmptyY;
        }

        gui::Panel::Update(state, offset);

        // If a widget is being dragged, check if we need to shift any
        // other widget up or down
        if (DragTarget != nullptr) {
            if (DragEmptyHeight == 0) {
                DragEmptyY = DragTargetInitialPosition.Y;
                DragEmptyHeight = DragTarget->Height;
            }

            // Find the widget that the drag target is intersecting with (if any)
            auto *dragTargetWap = GetWidgetAndPosition(DragTarget);
            AbortUnless(dragTargetWap != nullptr);
            gui::WidgetAndPosition *otherWap = nullptr;
            for (auto &wap : Widgets) {
                #pragma warning(suppress : 6011)
                if (std::get<0>(wap).get() != DragTarget &&
                    gui::WidgetsIntersect(*dragTargetWap, wap)) {
                    otherWap = &wap;
                    break;
                }
            }

            if (otherWap != nullptr) {
                const auto emptySpaceBelowDragTarget =
                        DragEmptyY > std::get<1>(*dragTargetWap).Y;
                const auto emptySpaceAboveDragTarget =
                        DragEmptyY < std::get<1>(*dragTargetWap).Y;
                const auto dragTargetAboveOther =
                        std::get<1>(*dragTargetWap).Y <
                        std::get<1>(*otherWap).Y +
                                (std::get<0>(*otherWap)->Height / 2);
                const auto dragTargetBelowOther =
                        std::get<1>(*dragTargetWap).Y + DragTarget->Height >
                        std::get<1>(*otherWap).Y +
                                (std::get<0>(*otherWap)->Height / 2);
                if (emptySpaceBelowDragTarget && dragTargetAboveOther) {
                    // Shift other widget down
                    const auto oldOtherY = std::get<1>(*otherWap).Y;
                    std::get<1>(*otherWap).Y =
                            DragEmptyY + DragEmptyHeight - std::get<0>(*otherWap)->Height;
                    DragEmptyY = oldOtherY;
                } else if (emptySpaceAboveDragTarget && dragTargetBelowOther) {
                    // Shift other widget up
                    const auto oldOtherY = std::get<1>(*otherWap).Y;
                    std::get<1>(*otherWap).Y = DragEmptyY;
                    DragEmptyY += std::get<0>(*otherWap)->Height;
                }
            }
        } else {
            // Reset DragEmptyHeight
            DragEmptyHeight = 0;

            // If no widget is being dragged then make sure that we have
            // the correct height set
            // (which can only change by minimizing/maximizing SidebarIventory)
            Height = 0;
            for (const auto &wap : Widgets) {
                Height = std::max(Height,
                                  std::get<1>(wap).Y + std::get<0>(wap)->Height + Border);
            }
        }
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        Common::DrawBorder2px(_Gamestate->Version.Icons,
                              canvas,
                              offset.X,
                              offset.Y,
                              offset.X + Width,
                              offset.Y + Height);

        // We can't render using Panel::Render since we need to render
        // the drag taget (if any) last
        for (const auto &wap : Widgets) {
            if (DragTarget != nullptr && std::get<0>(wap).get() == DragTarget) {
                continue;
            }
            std::get<0>(wap)->Render(canvas, offset + std::get<1>(wap));
        }
        if (DragTarget != nullptr) {
            DragTarget->Render(
                    canvas,
                    offset + std::get<1>(*GetWidgetAndPosition(DragTarget)));
        }
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
