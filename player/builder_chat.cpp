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

struct DummyWidget : public gui::Widget {
    DummyWidget(int width, int height) : Widget(width, height) {
    }

    void Render(Canvas &canvas, gui::Position offset) override {
    }
};

struct Chat : public gui::Widget {

    Gamestate *Gamestate_;
    gui::Border MessagesBorder;

    // I have no idea if 0 or 65535 are valid values, so just use a "random" value here...
    static constexpr uint16_t NO_ACTIVE_CHANNEL = 12345;
    uint16_t ActiveChannelId = NO_ACTIVE_CHANNEL;
    std::vector<uint16_t> ChannelOrder;
  
    Chat(int width, int height, Gamestate *gamestate)
        : Widget(width, height),
          Gamestate_(gamestate),
          MessagesBorder(&gamestate->Version.Icons,
                         gui::Border::BorderType::Raised,
                         std::make_unique<DummyWidget>(width - 4, height - 4 - 21)) {
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
                    ActiveChannelId = NO_ACTIVE_CHANNEL;
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

        // If we don't have an active channel, set the first one as active
        if (ActiveChannelId == NO_ACTIVE_CHANNEL && !ChannelOrder.empty()) {
            ActiveChannelId = ChannelOrder.front();
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
        MessagesBorder.Render(canvas, gui::Position(offset.X, offset.Y + 21));

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
        canvas.DrawTiled(icons.ClientBackground,
                         offset.X + 2,
                         offset.Y + 23,
                         offset.X + Width - 2,
                         offset.Y + Height - 2);
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
