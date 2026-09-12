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

#include <cstdint>
#include <memory>

#include <gui/border.hpp>
#include "gui/panel.hpp"
#include "gui/widget.hpp"
#include "canvas.hpp"
#include "gamestate.hpp"
#include "versions.hpp"
#include "textrenderer.hpp"

using namespace trc;

struct Chat : public gui::Widget {

    Gamestate *Gamestate_;

    uint16_t ActiveChannelId = Gamestate::DefaultChannelId;
    std::vector<uint16_t> ChannelOrder;

    // Separate canvas for messages to avoid redrawing the entire chat window
    // every frame, to support scrolling, etc
    Canvas MessagesCanvas;
  
    Chat(int width, int height, Gamestate *gamestate)
        : Widget(width, height),
          Gamestate_(gamestate),
          ChannelOrder({Gamestate::DefaultChannelId}),
          MessagesCanvas(width - 14, 14 * 100 /* 14 pixels per line, 100 lines in scrollback */) {
    }

    void Update(gui::State &state, gui::Position offset) override {
        // Quick check to see if any channel has been opened or closed
        bool channelsChanged = ChannelOrder.size() != Gamestate_->Channels.size();
        if (!channelsChanged) {
            for (const auto channelId : ChannelOrder) {
                if (Gamestate_->Channels.count(channelId) == 0) {
                    channelsChanged = true;
                    break;
                }
            }
        }
        if (!channelsChanged) {
            return;
        }

        // Remove any channels that have been closed
        for (auto it = ChannelOrder.begin(); it != ChannelOrder.end();) {
            if (Gamestate_->Channels.count(*it) == 0) {
                if (ActiveChannelId == *it) {
                    ActiveChannelId = Gamestate::DefaultChannelId;
                }
                it = ChannelOrder.erase(it);
            } else {
                ++it;
            }
        }

        // Add any channels that have been opened
        for (const auto &[id, channel] : Gamestate_->Channels) {
            if (std::find(ChannelOrder.begin(), ChannelOrder.end(), id) == ChannelOrder.end()) {
                ChannelOrder.push_back(id);
            }
        }
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
        auto x = offset.X + 18;
        for (const auto channelId : ChannelOrder) {
            const auto &channel = Gamestate_->Channels.at(channelId);
            canvas.Draw(channelId == ActiveChannelId ? icons.ChatChannelBoxActive : icons.ChatChannelBoxInactive, x, offset.Y + 5);
            TextRenderer::DrawCenteredString(fonts.Game,
                                             channelId == ActiveChannelId ? Pixel(0xDF, 0xDF, 0xDF) : Pixel(0x80, 0x80, 0x80),
                                             x + 48,
                                             offset.Y + 9,
                                             channel.Name,
                                             canvas);

            x += 96;
        }

        // Chat window
        canvas.Draw(icons.ChatMessageBorderTopLeft,
                    offset.X + 4,
                    offset.Y + 26);
        canvas.DrawTiled(icons.ChatMessageBorderHorizontal,
                         offset.X + 7,
                         offset.Y + 26,
                         offset.X + Width - 7,
                         offset.Y + 29);
        canvas.Draw(icons.ChatMessageBorderTopRight, offset.X + Width - 7, offset.Y + 26);
        canvas.DrawTiled(icons.ChatMessageBorderVertical,
                               offset.X + 4,
                               offset.Y + 29,
                               offset.X + 7,
                               offset.Y + Height - 25);
        canvas.DrawTiled(icons.ChatMessageBorderVertical,
                               offset.X + Width - 7,
                               offset.Y + 29,
                               offset.X + Width - 4,
                               offset.Y + Height - 25);
        canvas.Draw(icons.ChatMessageBorderBottomLeft, offset.X + 4, offset.Y + Height - 25);
        canvas.DrawTiled(icons.ChatMessageBorderHorizontal,
                               offset.X + 7,
                               offset.Y + Height - 25,
                               offset.X + Width - 7,
                               offset.Y + Height - 22);
        canvas.Draw(icons.ChatMessageBorderBottomRight,
                     offset.X + Width - 7,
                     offset.Y + Height - 25);

        // Chat messages
        // For now, always re-render MessagesCanvas, which probably should be converted to a Widget
        // so that we can implement scrolling
        MessagesCanvas.Wipe();
        MessagesCanvas.DrawTiled(icons.ClientBackground,
                                 0,
                                 0,
                                 MessagesCanvas.Width,
                                 MessagesCanvas.Height);

        // For now, render all messages to MessageCanvas (max 100 lines)
        // 14 pixels between rows, 2 pixels from the left edge
        int y = 14 * 100 - 14;
        const auto &channel = Gamestate_->Channels.at(ActiveChannelId);
        for (auto it = channel.Messages.rbegin(); it != channel.Messages.rend(); ++it) {
            const auto textColor = [&]() -> Pixel {
                switch (it->Mode) {
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
            y -= 14;
            if (y < 0) {
                break;
            }
        }

        // Calculate how much room we have
        int availableHeight = Height - 54;
        if (availableHeight > MessagesCanvas.Height) {
            availableHeight = MessagesCanvas.Height;
        }
        Canvas::Copy(canvas,
                     MessagesCanvas,
                     0,
                     MessagesCanvas.Height - availableHeight,
                     MessagesCanvas.Width,
                     MessagesCanvas.Height,
                     offset.X + 7,
                     offset.Y + 29);

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

    void SetLayoutSize(int width, int height) {
        Widget::SetLayoutSize(width, height);
        MessagesCanvas = Canvas(width - 14, 14 * 100);
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
