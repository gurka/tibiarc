/*
 * Copyright 2011-2016 "Silver Squirrel Software Handelsbolag"
 * Copyright 2023-2024 "John Högberg"
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

#include "ui_renderer.hpp"

#include <cmath>
#include <cstdlib>
#include <initializer_list>
#include <tuple>

#include "versions.hpp"
#include "fonts.hpp"
#include "textrenderer.hpp"
#include "renderer.hpp"
#include "utils.hpp"

namespace trc {

namespace UiRenderer {

void DrawBorder1px(const Icons &icons,
                   Canvas &canvas,
                   int leftX,
                   int topY,
                   int rightX,
                   int bottomY) noexcept {
    canvas.DrawBackground(icons.BorderHorizontalDark,
                          leftX,
                          topY,
                          rightX,
                          topY + 1);
    canvas.DrawBackground(icons.BorderVerticalDark,
                          leftX,
                          topY + 1,
                          leftX + 1,
                          bottomY - 1);
    canvas.DrawBackground(icons.BorderVerticalLight,
                          rightX - 1,
                          topY + 1,
                          rightX,
                          bottomY - 1);
    canvas.DrawBackground(icons.BorderHorizontalLight,
                          leftX,
                          bottomY - 1,
                          rightX,
                          bottomY);
}

void DrawBorder2px(const Icons &icons,
                   Canvas &canvas,
                   int leftX,
                   int topY,
                   int rightX,
                   int bottomY) noexcept {
    canvas.DrawBackground(icons.BorderCornerLight,
                          leftX,
                          topY,
                          leftX + 2,
                          topY + 2);
    canvas.DrawBackground(icons.BorderHorizontalLight,
                          leftX + 2,
                          topY,
                          rightX - 2,
                          topY + 2);
    canvas.DrawBackground(icons.BorderCornerLightDark,
                          rightX - 2,
                          topY,
                          rightX,
                          topY + 2);
    canvas.DrawBackground(icons.BorderVerticalLight,
                          leftX,
                          topY + 2,
                          leftX + 2,
                          bottomY - 2);
    canvas.DrawBackground(icons.BorderVerticalDark,
                          rightX - 2,
                          topY + 2,
                          rightX,
                          bottomY - 2);
    canvas.DrawBackground(icons.BorderCornerLightDark,
                          leftX,
                          bottomY - 2,
                          leftX + 2,
                          bottomY);
    canvas.DrawBackground(icons.BorderHorizontalDark,
                          leftX + 2,
                          bottomY - 2,
                          rightX - 2,
                          bottomY);
    canvas.DrawBackground(icons.BorderCornerDark,
                          rightX - 2,
                          bottomY - 2,
                          rightX,
                          bottomY);
}

int DrawSidebarTop(Gamestate &gamestate, Canvas &canvas) noexcept {
    const auto &icons = gamestate.Version.Icons;

    // Size: 176x334 (assuming inventory area is not minimized)

    DrawBorder2px(icons, canvas, 0, 0, 176, 334);

    // Draw each part
    // For now don't support changing area order (in the player)
    int offsetY = 2;
    DrawMinimapArea(gamestate, canvas, offsetY);
    DrawStatusBars(gamestate, canvas, offsetY);
    DrawInventoryArea(gamestate, canvas, offsetY);
    DrawWindowButtons(gamestate, canvas, offsetY);
    offsetY += 2;
    return offsetY;
}

void DrawMinimapArea(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept {
    const auto &fonts = gamestate.Version.Fonts;
    const auto &icons = gamestate.Version.Icons;

    // Size: 172x117
    canvas.DrawBackground(gamestate.Version.Icons.ClientBackground,
                          2,
                          offsetY,
                          2 + 172,
                          offsetY + 117);

    // Minimap, empty for now
    DrawBorder1px(icons, canvas, 10, offsetY + 4, 10 + 108, offsetY + 4 + 108);
    canvas.DrawRectangle(Pixel(0, 0, 0), 11, offsetY + 5, 106, 106);

    // Buttons
    canvas.Draw(icons.Compass, 126, offsetY + 5);
    canvas.Draw(icons.ZoomOut, 126, offsetY + 50);
    canvas.Draw(icons.LevelUp, 149, offsetY + 50);
    canvas.Draw(icons.ZoomIn, 126, offsetY + 71);
    canvas.Draw(icons.LevelDown, 149, offsetY + 71);
    canvas.Draw(icons.Button43px, 126, offsetY + 92);
    TextRenderer::DrawCenteredString(fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     147,
                                     offsetY + 98,
                                     "Centre",
                                     canvas);

    // Adjust offsetY for next area
    offsetY += 117;
}

void DrawStatusBars(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept {
    const auto &icons = gamestate.Version.Icons;
    const auto &fonts = gamestate.Version.Fonts;

    // Size: 172x32
    canvas.DrawBackground(gamestate.Version.Icons.ClientBackground,
                          2,
                          offsetY,
                          2 + 172,
                          offsetY + 32);

    // Health
    canvas.Draw(icons.HealthIcon, 11, offsetY + 5);
    canvas.Draw(icons.EmptyStatusBar, 28, offsetY + 4);
    if (gamestate.Player.Stats.MaxHealth > 0 &&
        gamestate.Player.Stats.Health <= gamestate.Player.Stats.MaxHealth) {
        canvas.Draw(icons.HealthBar,
                    28,
                    offsetY + 4,
                    (icons.HealthBar.Width * gamestate.Player.Stats.Health) /
                            gamestate.Player.Stats.MaxHealth,
                    11);
    }
    TextRenderer::DrawString(fonts.InterfaceLarge,
                             Pixel(0xAF, 0xAF, 0xAF),
                             129,
                             offsetY + 5,
                             std::to_string(gamestate.Player.Stats.Health),
                             canvas);

    // Mana
    canvas.Draw(icons.ManaIcon, 11, offsetY + 18);
    canvas.Draw(icons.EmptyStatusBar, 28, offsetY + 17);
    if (gamestate.Player.Stats.MaxMana > 0 &&
        gamestate.Player.Stats.Mana <= gamestate.Player.Stats.MaxMana) {
        canvas.Draw(icons.ManaBar,
                    28,
                    offsetY + 17,
                    (icons.ManaBar.Width * gamestate.Player.Stats.Mana) /
                            gamestate.Player.Stats.MaxMana,
                    11);
    }
    TextRenderer::DrawString(fonts.InterfaceLarge,
                             Pixel(0xAF, 0xAF, 0xAF),
                             129,
                             offsetY + 17,
                             std::to_string(gamestate.Player.Stats.Mana),
                             canvas);

    // Adjust offsetY for next area
    offsetY += 32;
}

void DrawInventoryArea(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept {
    const Version &version = gamestate.Version;
    const Icons &icons = version.Icons;

    // Size: 172x155
    canvas.DrawBackground(gamestate.Version.Icons.ClientBackground,
                          2,
                          offsetY,
                          2 + 172,
                          offsetY + 155);

    // Inventory
    canvas.Draw(icons.Minimize, 10, offsetY + 4);
    for (auto [slot, x, y] :
         std::initializer_list<std::tuple<InventorySlot, int, int>>{
                 {InventorySlot::Head, 47, offsetY + 4},
                 {InventorySlot::Amulet, 10, offsetY + 18},
                 {InventorySlot::Backpack, 84, offsetY + 18},
                 {InventorySlot::Chest, 47, offsetY + 41},
                 {InventorySlot::RightArm, 10, offsetY + 55},
                 {InventorySlot::LeftArm, 84, offsetY + 55},
                 {InventorySlot::Legs, 47, offsetY + 78},
                 {InventorySlot::Ring, 10, offsetY + 92},
                 {InventorySlot::Quiver, 84, offsetY + 92},
                 {InventorySlot::Boots, 47, offsetY + 115},
         }) {
        Renderer::DrawInventorySlot(gamestate, slot, x, y, canvas);
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

    canvas.Draw(icons.SecondaryStatBackground, 84, offsetY + 128);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     101,
                                     offsetY + 130,
                                     "Cap:",
                                     canvas);
    uint32_t capacity =
            gamestate.Player.Stats.Capacity / version.Features.CapacityDivisor;
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceLarge,
                                     Pixel(0xBF, 0xBF, 0xBF),
                                     101,
                                     offsetY + 139,
                                     std::to_string(capacity),
                                     canvas);

    // Attack mode buttons
    canvas.Draw(icons.FightingOffensive, 126, offsetY + 19);
    canvas.Draw(icons.FightingBalanced, 126, offsetY + 39);
    canvas.Draw(icons.FightingDefensive, 126, offsetY + 59);
    canvas.Draw(icons.AttackStanding, 149, offsetY + 19);
    canvas.Draw(icons.AttackChasing, 149, offsetY + 39);
    canvas.Draw(icons.AttackUnmarked, 149, offsetY + 59);

    // Buttons
    canvas.Draw(icons.Button43px, 126, offsetY + 83);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     147,
                                     offsetY + 90,
                                     "Stop",
                                     canvas);

    canvas.Draw(icons.Button43px, 126, offsetY + 107);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     147,
                                     offsetY + 114,
                                     "Options",
                                     canvas);


    canvas.Draw(icons.Button43px, 126, offsetY + 131);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     147,
                                     offsetY + 138,
                                     "Help",
                                     canvas);

    // Adjust offsetY for next area
    offsetY += 155;
}

void DrawWindowButtons(Gamestate& gamestate, Canvas& canvas, int &offsetY) noexcept {
    const Version &version = gamestate.Version;
    const Icons &icons = version.Icons;

    // Size: 172x26
    canvas.DrawBackground(gamestate.Version.Icons.ClientBackground,
                          2,
                          offsetY,
                          2 + 172,
                          offsetY + 26);
    canvas.Draw(icons.Button34px, 10, offsetY + 3);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     27,
                                     offsetY + 9,
                                     "Skills",
                                     canvas);

    canvas.Draw(icons.Button34px, 47, offsetY + 3);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     64,
                                     offsetY + 9,
                                     "Battle",
                                     canvas);

    canvas.Draw(icons.Button34px, 84, offsetY + 3);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     101,
                                     offsetY + 9,
                                     "VIP",
                                     canvas);

    canvas.Draw(icons.Button43px, 126, offsetY + 3);
    TextRenderer::DrawCenteredString(version.Fonts.InterfaceSmall,
                                     Pixel(0xFF, 0xFF, 0xFF),
                                     147,
                                     offsetY + 9,
                                     "Logout",
                                     canvas);

    // Adjust offsetY for next area
    offsetY += 26;
}

void DrawSidebarMiddle(Gamestate &gamestate,
                       Canvas &canvas,
                       int &offsetY) noexcept {
    DrawSkillsWindow(gamestate, canvas, offsetY);
    DrawBattleWindow(gamestate, canvas, offsetY);
}

void DrawSidebarWindowBackground(Gamestate &gamestate,
                                 Canvas &canvas,
                                 int offsetY,
                                 int height) noexcept {
    AbortUnless(height >= 57);

    const auto &icons = gamestate.Version.Icons;

    canvas.DrawBackground(icons.ClientBackground,
                          4,
                          offsetY + 15,
                          4 + 156,
                          offsetY + 15 + height - 19);
}

void DrawSidebarWindow(Gamestate &gamestate,
                       Canvas &canvas,
                       int offsetY,
                       const Sprite &icon,
                       const std::string &title,
                       int height) noexcept {
    AbortUnless(height >= 57);

    const auto &icons = gamestate.Version.Icons;
    const auto &fonts = gamestate.Version.Fonts;

    // Header
    canvas.Draw(icons.WindowHeaderLeft, 0, offsetY);
    canvas.DrawBackground(icons.WindowHeaderMiddle,
                          4,
                          offsetY,
                          176 - 4,
                          offsetY + 15);
    canvas.Draw(icons.WindowHeaderRight, 176 - 4, offsetY);
    canvas.Draw(icon, 4, offsetY + 2);
    TextRenderer::DrawString(fonts.InterfaceLarge,
                             Pixel(0x8F, 0x8F, 0x8F),
                             20,
                             offsetY + 4,
                             title,
                             canvas);
    canvas.Draw(icons.Minimize, 148, offsetY + 2);
    canvas.Draw(icons.Close, 161, offsetY + 2);

    // Middle
    canvas.DrawBackground(icons.WindowLeft,
                          0,
                          offsetY + 15,
                          icons.WindowLeft.Width,
                          offsetY + 15 + height - 19);
    canvas.DrawBackground(icons.WindowRight,
                          176 - icons.WindowRight.Width,
                          offsetY + 15,
                          176,
                          offsetY + 15 + height - 19);

    // Scrollbar
    canvas.Draw(icons.ScrollbarUp, 160, offsetY + 15);
    canvas.DrawBackground(icons.ScrollbarBackground,
                          160,
                          offsetY + 27,
                          160 + 12,
                          offsetY + 27 + height - 43);
    canvas.Draw(icons.ScrollbarButton, 160, offsetY + 27);
    canvas.Draw(icons.ScrollbarDown, 160, offsetY + height - 16);

    // Bottom
    canvas.Draw(icons.WindowBottomLeft, 0, offsetY + 15 + height - 19);
    canvas.DrawBackground(icons.WindowBottom,
                          4,
                          offsetY + 15 + height - 19,
                          176 - 4,
                          offsetY + 15 + height - 19 + 4);
    canvas.Draw(icons.WindowBottomRight, 176 - 4, offsetY + 15 + height - 19);
    canvas.Draw(icons.WindowResize, 3, offsetY + 15 + height - 19 - 12);
}

void DrawSkillsWindow(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept {
    const auto &icons = gamestate.Version.Icons;
    const auto &fonts = gamestate.Version.Fonts;

    DrawSidebarWindowBackground(gamestate, canvas, offsetY, 57);

    TextRenderer::DrawString(fonts.InterfaceLarge,
                             Pixel(0xAF, 0xAF, 0xAF),
                             14,
                             offsetY + 25,
                             "Experience",
                             canvas);
    TextRenderer::DrawRightAlignedString(fonts.InterfaceLarge,
                                         Pixel(0xAF, 0xAF, 0xAF),
                                         149,
                                         offsetY + 25,
                                         ThousandSeparators(gamestate.Player.Stats.Experience),
                                         canvas);

    TextRenderer::DrawString(fonts.InterfaceLarge,
                             Pixel(0xAF, 0xAF, 0xAF),
                             14,
                             offsetY + 39,
                             "Level",
                             canvas);
    TextRenderer::DrawRightAlignedString(fonts.InterfaceLarge,
                                         Pixel(0xAF, 0xAF, 0xAF),
                                         149,
                                         offsetY + 39,
                                         ThousandSeparators(gamestate.Player.Stats.Level),
                                         canvas);

    DrawSidebarWindow(gamestate,
                      canvas,
                      offsetY,
                      icons.SkillsIcon,
                      "Skills",
                      57);

    // Adjust offsetY
    offsetY += 57;
}

void DrawBattleWindow(Gamestate &gamestate, Canvas &canvas, int &offsetY) noexcept {
    const auto &icons = gamestate.Version.Icons;
    const auto &fonts = gamestate.Version.Fonts;

    DrawSidebarWindowBackground(gamestate, canvas, offsetY, 57);

    DrawSidebarWindow(gamestate,
                      canvas,
                      offsetY,
                      icons.BattleIcon,
                      "Battle",
                      57);

    // Adjust offsetY
    offsetY += 57;
}

void DrawSidebarBottom(Gamestate &gamestate, Canvas &canvas, int offsetY) noexcept {
    const auto &icons = gamestate.Version.Icons;

    int height = canvas.Height - offsetY;

    // TODO: handle too low height, i.e. < 5 (2px border + 1px bg + 2px border)
    
    DrawBorder2px(icons, canvas, 0, offsetY, 176, offsetY + height);

    // TODO: we need a function to draw this sprite as a background but backwards (from bottom and up)
    canvas.DrawBackground(icons.ClientBackground,
                          2,
                          offsetY + 2,
                          176 - 2,
                          offsetY + height - 2);
}

void DrawChat(Gamestate &gamestate, Canvas &canvas) noexcept {
    const auto &icons = gamestate.Version.Icons;

    // Top border
    canvas.DrawBackground(icons.BorderHorizontalLight, 0, 0, canvas.Width, 1);
    canvas.DrawBackground(icons.ClientBackground, 0, 1, canvas.Width, 4);
    canvas.DrawBackground(icons.BorderHorizontalDark, 0, 4, canvas.Width, 5);

    // Window names background
    canvas.Draw(icons.ChatBackgroundDarkLeft, 0, 5);
    canvas.DrawBackground(icons.ChatBackgroundDark, 2, 5, canvas.Width, 21);

    // Buttons
    canvas.Draw(icons.ChatChannelButton, canvas.Width - 32, 5);
    canvas.Draw(icons.ChatIgnoreButton, canvas.Width - 16, 5);

    // Message window border
    DrawBorder2px(icons, canvas, 0, 21, canvas.Width, canvas.Height);

    canvas.Draw(icons.ChatChannelBoxActive, 18, 5);

    // TODO: spacing is 0 but it still too much, maybe we need a Chat font with -1 spacing?
    //       verify if this is for all text in chat or only chat window title
    TextRenderer::DrawCenteredString(gamestate.Version.Fonts.Game,
                                     Pixel(0xDF, 0xDF, 0xDF),
                                     66,
                                     10,
                                     "Default",
                                     canvas);

    // Message window background
    canvas.DrawBackground(icons.ClientBackground,
                          2,
                          23,
                          canvas.Width - 2,
                          canvas.Height - 2);

    // Message border
    canvas.Draw(icons.ChatMessageBorderTopLeft, 4, 26);
    canvas.DrawBackground(icons.ChatMessageBorderHorizontal,
                          7,
                          26,
                          canvas.Width - 7,
                          29);
    canvas.Draw(icons.ChatMessageBorderTopRight, canvas.Width - 7, 26);
    canvas.DrawBackground(icons.ChatMessageBorderVertical,
                          4,
                          29,
                          7,
                          canvas.Height - 25);
    canvas.DrawBackground(icons.ChatMessageBorderVertical,
                          canvas.Width - 7,
                          29,
                          canvas.Width - 4,
                          canvas.Height - 25);
    canvas.Draw(icons.ChatMessageBorderBottomLeft, 4, canvas.Height - 25);
    canvas.DrawBackground(icons.ChatMessageBorderHorizontal,
                          7,
                          canvas.Height - 25,
                          canvas.Width - 7,
                          canvas.Height - 22);
    canvas.Draw(icons.ChatMessageBorderBottomRight, canvas.Width - 7, canvas.Height - 25);

    // Scrollbar
    canvas.Draw(icons.ScrollbarUp, canvas.Width - 19, 29);
    canvas.DrawBackground(icons.ScrollbarBackground,
                          canvas.Width - 19,
                          29 + 12,
                          canvas.Width - 19 + 12,
                          canvas.Height - 37);
    canvas.Draw(icons.ScrollbarDown, canvas.Width - 19, canvas.Height - 37);

    // Message input box
    canvas.Draw(icons.ChatTalkButton, 5, canvas.Height - 20);
    DrawBorder1px(icons,
                  canvas,
                  23,
                  canvas.Height - 20,
                  canvas.Width - 4,
                  canvas.Height - 4);
    canvas.DrawRectangle(Pixel(0x36, 0x36, 0x36),
                         24,
                         canvas.Height - 19,
                         canvas.Width - 29,
                         14);
}

} // namespace UiRenderer
} // namespace trc
