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

#include "builder_chat.hpp"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <gui/border.hpp>
#include "gui/panel.hpp"
#include "gui/widget.hpp"
#include "canvas.hpp"
#include "gamestate.hpp"
#include "versions.hpp"
#include "textrenderer.hpp"

using namespace trc;

struct ChatChannelWindow : public gui::Widget {

    static constexpr int MaxMessageRows = 100;
    static constexpr int MessageRowHeight = 14;
    static constexpr int BorderThickness = 3;

    Gamestate *Gamestate_;
    uint16_t ChannelId_;

    Canvas MessagesCanvas;

    ChatChannelWindow(int width,
                      int height,
                      Gamestate *gamestate,
                      uint16_t channelId)
        : Widget(width, height),
          Gamestate_(gamestate),
          ChannelId_(channelId),
          MessagesCanvas(width - (2 * BorderThickness), MessageRowHeight * MaxMessageRows) {
    }

    void Update(gui::State &, gui::Position) override {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &fonts = Gamestate_->Version.Fonts;
        const auto &icons = Gamestate_->Version.Icons;

        // Chat window border
        canvas.Draw(icons.ChatMessageBorderTopLeft,
                    offset.X,
                    offset.Y);
        canvas.DrawTiled(icons.ChatMessageBorderHorizontal,
                         offset.X + BorderThickness,
                         offset.Y,
                         offset.X + Width - BorderThickness,
                         offset.Y + BorderThickness);
        canvas.Draw(icons.ChatMessageBorderTopRight,
                    offset.X + Width - BorderThickness,
                    offset.Y);
        canvas.DrawTiled(icons.ChatMessageBorderVertical,
                         offset.X,
                         offset.Y + BorderThickness,
                         offset.X + BorderThickness,
                         offset.Y + Height - BorderThickness);
        canvas.DrawTiled(icons.ChatMessageBorderVertical,
                         offset.X + Width - BorderThickness,
                         offset.Y + BorderThickness,
                         offset.X + Width,
                         offset.Y + Height - BorderThickness);
        canvas.Draw(icons.ChatMessageBorderBottomLeft,
                    offset.X,
                    offset.Y + Height - BorderThickness);
        canvas.DrawTiled(icons.ChatMessageBorderHorizontal,
                         offset.X + BorderThickness,
                         offset.Y + Height - BorderThickness,
                         offset.X + Width - BorderThickness,
                         offset.Y + Height);
        canvas.Draw(icons.ChatMessageBorderBottomRight,
                    offset.X + Width - BorderThickness,
                    offset.Y + Height - BorderThickness);

        MessagesCanvas.Wipe();
        MessagesCanvas.DrawTiled(icons.ClientBackground,
                                 0,
                                 0,
                                 MessagesCanvas.Width,
                                 MessagesCanvas.Height);

        const auto &channel = Gamestate_->Channels.at(ChannelId_);
        int y = MessageRowHeight * MaxMessageRows - MessageRowHeight;
        for (auto it = channel.Messages.rbegin(); it != channel.Messages.rend(); ++it) {
            const auto textColor = [&]() -> Pixel {
                switch (it->Mode) {
                // TODO: There are more colors...
                case MessageMode::Say:
                case MessageMode::Whisper:
                case MessageMode::Yell:
                    return Pixel(0xEF, 0xEF, 0x00);
                case MessageMode::PrivateIn:
                case MessageMode::PrivateOut:
                    return Pixel(0x9F, 0x9F, 0xFE);
                case MessageMode::Loot:
                    return Pixel(0x00, 0xEF, 0x00);
                default:
                    return Pixel(0xDF, 0xDF, 0xDF);
                }
            }();
            TextRenderer::DrawString(fonts.Game,
                                     textColor,
                                     2,
                                     y,
                                     (it->AuthorName.empty() ? "" : (it->AuthorName + ": ")) + it->Message,
                                     MessagesCanvas);
            y -= MessageRowHeight;
            if (y < 0) {
                break;
            }
        }

        int availableHeight = Height - (2 * BorderThickness);
        if (availableHeight > MessagesCanvas.Height) {
            availableHeight = MessagesCanvas.Height;
        }
        Canvas::Copy(canvas,
                     MessagesCanvas,
                     0,
                     MessagesCanvas.Height - availableHeight,
                     MessagesCanvas.Width,
                     MessagesCanvas.Height,
                     offset.X + BorderThickness,
                     offset.Y + BorderThickness);
    }

    void SetLayoutSize(int width, int height) override {
        Widget::SetLayoutSize(width, height);
        MessagesCanvas = Canvas(width - (2 * BorderThickness), MessageRowHeight * MaxMessageRows);
    }
};

struct Chat : public gui::Widget {

    static constexpr int ChannelTabsX = 18;
    static constexpr int ChannelTabsY = 5;
    static constexpr int ChannelTabStride = 96;

    Gamestate *Gamestate_;

    uint16_t ActiveChannelId = Gamestate::DefaultChannelId;
    uint16_t PressedChannelId = ~0;

    std::vector<uint16_t> ChannelOrder;
    std::unordered_map<uint16_t, std::unique_ptr<ChatChannelWindow>> ChannelWidgets;

    Chat(int width, int height, Gamestate *gamestate)
        : Widget(width, height),
          Gamestate_(gamestate),
          ChannelOrder({Gamestate::DefaultChannelId}) {
        ChannelWidgets.emplace(Gamestate::DefaultChannelId,
                               std::make_unique<ChatChannelWindow>(width - 8,
                                                                   height - 48,
                                                                   Gamestate_,
                                                                   Gamestate::DefaultChannelId));
    }

    void Update(gui::State &state, gui::Position offset) override {
        // Quick check to see if any channel has been opened or closed
        bool channelsChanged =
                ChannelOrder.size() != Gamestate_->Channels.size();
        if (!channelsChanged) {
            for (const auto channelId : ChannelOrder) {
                if (Gamestate_->Channels.count(channelId) == 0) {
                    channelsChanged = true;
                    break;
                }
            }
        }

        if (channelsChanged) {
            // Remove any channels that have been closed
            for (auto it = ChannelOrder.begin(); it != ChannelOrder.end();) {
                if (Gamestate_->Channels.count(*it) == 0) {
                    if (ActiveChannelId == *it) {
                        ActiveChannelId = Gamestate::DefaultChannelId;
                    }
                    ChannelWidgets.erase(*it);
                    it = ChannelOrder.erase(it);
                } else {
                    ++it;
                }
            }

            // Add any channels that have been opened
            for (const auto &[id, channel] : Gamestate_->Channels) {
                if (std::find(ChannelOrder.begin(), ChannelOrder.end(), id) ==
                    ChannelOrder.end()) {
                    ChannelOrder.push_back(id);
                    ChannelWidgets.emplace(
                            id,
                            std::make_unique<ChatChannelWindow>(Width - 8,
                                                                Height - 48,
                                                                Gamestate_,
                                                                id));
                }
            }
        }

        ChannelWidgets.at(ActiveChannelId)->Update(state, offset);
    }

    void Render(Canvas &canvas, gui::Position offset) override {
        const auto &fonts = Gamestate_->Version.Fonts;
        const auto &icons = Gamestate_->Version.Icons;

        // Top border
        canvas.DrawTiled(icons.BorderHorizontalLight,
                         offset.X,
                         offset.Y,
                         offset.X + Width,
                         offset.Y + 1);
        canvas.DrawTiled(icons.BorderHorizontalDark,
                         offset.X,
                         offset.Y + 4,
                         offset.X + Width,
                         offset.Y + 5);

        // Channels background
        canvas.Draw(icons.ChatBackgroundDarkLeft, offset.X, offset.Y + 5);
        canvas.DrawTiled(icons.ChatBackgroundDark,
                         offset.X + 2,
                         offset.Y + 5,
                         offset.X + Width,
                         offset.Y + 5 + icons.ChatBackgroundDark.Height);

        // Buttons
        canvas.Draw(icons.ChatChannelButton,
                    offset.X + Width - 32,
                    offset.Y + 5);
        canvas.Draw(icons.ChatIgnoreButton,
                    offset.X + Width - 16,
                    offset.Y + 5);

        // Channels border
        gui::Border::RenderRaisedBorder(icons,
                                        canvas,
                                        gui::Position(offset.X, offset.Y + 21),
                                        Width,
                                        Height - 21);

        // Channels
        auto x = offset.X + ChannelTabsX;
        for (const auto channelId : ChannelOrder) {
            canvas.Draw(channelId == ActiveChannelId
                                ? icons.ChatChannelBoxActive
                                : icons.ChatChannelBoxInactive,
                        x,
                        offset.Y + ChannelTabsY);
            TextRenderer::DrawCenteredString(
                    fonts.Game,
                    channelId == ActiveChannelId ? Pixel(0xDF, 0xDF, 0xDF)
                                                 : Pixel(0x80, 0x80, 0x80),
                    x + 48,
                    offset.Y + 9,
                    Gamestate_->Channels.at(channelId).Name,
                    canvas);

            x += ChannelTabStride;
        }

        ChannelWidgets.at(ActiveChannelId)
                ->Render(canvas, offset + gui::Position(4, 26));

        // Chat input box
        canvas.Draw(icons.ChatTalkButton, 5, offset.Y + Height - 20);
        gui::Border::RenderSunkenBorder(
                icons,
                canvas,
                gui::Position(offset.X + 23, offset.Y + Height - 20),
                Width - 27,
                16);
        canvas.DrawRectangle(Pixel(0x36, 0x36, 0x36),
                             offset.X + 24,
                             offset.Y + Height - 19,
                             Width - 29,
                             14);
    }

    MouseEventResult OnMouseEvent(gui::Widget::MouseEvent event,
                                  gui::Position position) override {
        const auto hitTestChannel = [&](gui::Position p) -> const uint16_t * {
            int x = ChannelTabsX;
            for (const auto &channelId : ChannelOrder) {
                if (p.X >= x && p.X < x + 96 && p.Y >= ChannelTabsY &&
                    p.Y < ChannelTabsY + 18) {
                    return &channelId;
                }
                x += ChannelTabStride;
            }
            return nullptr;
        };

        if (event == gui::Widget::MouseEvent::LeftDown) {
            PressedChannelId = ~0;
            if (const auto *channelId = hitTestChannel(position)) {
                PressedChannelId = *channelId;
                return MouseEventResult::Handled;
            }
            return MouseEventResult::NotHandled;
        }

        if (event == gui::Widget::MouseEvent::LeftUp) {
            if (PressedChannelId != ~0) {
                const auto *channelId = hitTestChannel(position);
                if (channelId != nullptr && *channelId == PressedChannelId) {
                    ActiveChannelId = PressedChannelId;
                }
            }
            PressedChannelId = ~0;
            return MouseEventResult::Handled;
        }

        return MouseEventResult::NotHandled;
    }

    void SetLayoutSize(int width, int height) override {
        Widget::SetLayoutSize(width, height);
        for (auto &channelWidget : ChannelWidgets) {
            channelWidget.second->SetLayoutSize(width - 8, height - 48);
        }
    }
};

struct ChatPanel : public gui::Panel {

    Chat *Chat_;

    ChatPanel(int width,
              int height,
              Gamestate *gamestate,
              std::unique_ptr<Chat> chat)
        : Panel(width, height), Chat_(chat.get()) {
        Add(std::move(chat), gui::Position(0, 0));
        SetBackground(&gamestate->Version.Icons.ClientBackground);
    }

    void SetLayoutSize(int width, int height) override {
        Panel::SetLayoutSize(width, height);
        Chat_->SetLayoutSize(width, height);
    }
};

std::unique_ptr<gui::Widget> Builder::BuildChat(int width,
                                                int height,
                                                Gamestate *gamestate) {
    return std::make_unique<ChatPanel>(
            width,
            height,
            gamestate,
            std::make_unique<Chat>(width, height, gamestate));
}
