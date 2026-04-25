/*
 * Copyright 2024-2026 "Simon Sandström"
 * Copyright 2024-2025 "John Högberg"
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

#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>

#ifdef EMSCRIPTEN
#    include <emscripten.h>
#endif
#include <SDL2/SDL.h>

#include "gui/gui.hpp"
#include "versions.hpp"
#include "memoryfile.hpp"

namespace {

std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window{
        nullptr,
        &SDL_DestroyWindow};
std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer{
        nullptr,
        &SDL_DestroyRenderer};

trc::gui::Gui gui;
std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> gui_texture{
        nullptr,
        &SDL_DestroyTexture};

std::unique_ptr<trc::Version> version;
}

void handle_resize() {
    int width;
    int height;
    SDL_GetRendererOutputSize(renderer.get(), &width, &height);
    gui.Resize(width, height);
    gui_texture.reset(SDL_CreateTexture(renderer.get(),
                                        SDL_PIXELFORMAT_RGBA32,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        width,
                                        height));
}

void handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                handle_resize();
            }
            break;
        case SDL_QUIT:
            exit(0);
        default:
            break;
        }
    }
}

void main_loop() {
    handle_input();

    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(renderer.get());

    // Render gui to texture
    if (SDL_LockTexture(gui_texture.get(),
                         nullptr,
                         (void **)&gui.Canvas->Buffer,
                         &gui.Canvas->Stride) != 0) {
        std::cerr << "Failed to lock texture: " << SDL_GetError() << std::endl;
        exit(1);
    }
    gui.Render(*version);
    SDL_UnlockTexture(gui_texture.get());

    // Render texture to window
    SDL_RenderCopy(renderer.get(), gui_texture.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer.get());
}


#ifndef EMSCRIPTEN
void emscripten_set_main_loop(const std::function<void(void)>& main_loop,
                              int fps,
                              int simulate_infinite_loop) {
    if (fps == 0) {
        fps = 120;
    }

    Uint32 ms_per_iteration = 1000 / fps;
    while (true) {
        Uint32 start = SDL_GetTicks();
        main_loop();
        if (simulate_infinite_loop == 0) {
            break;
        }
        Uint32 elapsed = SDL_GetTicks() - start;
        if (elapsed < ms_per_iteration) {
            SDL_Delay(ms_per_iteration - elapsed);
        }
    }
}
#endif

int main(int argc, char *argv[]) {
    int major = 0;
    int minor = 0;

    if (argc < 2 || argc > 3) {
        std::cout << "usage: " << argv[0] << " DATA_FOLDER [VERSION]"
                  << std::endl;
        return 1;
    }

    if (argc == 3) {
        if (sscanf(argv[2], "%u.%u", &major, &minor) < 2) {
            std::cerr << "version must be in the format 'X.Y', e.g. '8.55'"
                      << std::endl;
            return 1;
        }
    }

    try {
        const std::filesystem::path dataFolder = argv[1];
        const trc::MemoryFile pictures(dataFolder / "Tibia.pic");
        const trc::MemoryFile sprites(dataFolder / "Tibia.spr");
        const trc::MemoryFile types(dataFolder / "Tibia.dat");
        version = std::make_unique<trc::Version>(
                trc::VersionTriplet(major, minor, 0),
                pictures.Reader(),
                sprites.Reader(),
                types.Reader());

    } catch (const trc::ErrorBase &error) {
        std::cerr << "Unrecoverable error (" << error.Description() << ")"
                  << std::endl;
        return 1;
    }

    window.reset(SDL_CreateWindow("tibiarc GUI test",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  800,
                                  600,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
    if (!window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    renderer.reset(SDL_CreateRenderer(window.get(),
                                      -1,
                                      SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC |
                                              SDL_RENDERER_TARGETTEXTURE));
    if (!renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError()
                  << std::endl;
        return 1;
    }

    if (SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND) != 0) {
        std::cerr << "Failed to set blend mode: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    handle_resize();

    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}