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

#include "ui_common.hpp"

#include "icons.hpp"
#include "canvas.hpp"

namespace trc {
namespace UiCommon {

Wrapper<SDL_Texture> CreateTexture(SDL_Renderer *renderer,
                                   int width,
                                   int height) {
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             width,
                                             height);

    AbortUnless(texture != nullptr);
    AbortUnless(!SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND));

    return Wrapper<SDL_Texture>(texture, SDL_DestroyTexture);
}

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

} // namespace UiCommon
} // namespace trc
