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
#include <tuple>

#include <SDL2/SDL.h>

#include "gui/button.hpp"
#include "gui/gui.hpp"
#include "gui/position.hpp"
#include "gui/state.hpp"
#include "gui/window.hpp"

#include "memoryfile.hpp"
#include "pixel.hpp"
#include "versions.hpp"

using namespace trc;

namespace {

std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> Window{
        nullptr,
        &SDL_DestroyWindow};
std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> Renderer{
        nullptr,
        &SDL_DestroyRenderer};

gui::Gui Gui;
struct GuiState : public gui::State {
    int _MouseX = 0;
    int _MouseY = 0;
    bool _MouseLeftDown = false;

    gui::Position MousePosition() const override {
        return gui::Position(_MouseX, _MouseY);
    }

    bool MouseLeftDown() const override {
        return _MouseLeftDown;
    }
} GuiState;
std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> GuiTexture{
        nullptr,
        &SDL_DestroyTexture};

std::unique_ptr<Version> _Version;
}

void handle_resize() {
    int width;
    int height;
    SDL_GetRendererOutputSize(Renderer.get(), &width, &height);
    
    Gui.Resize(width, height);
    Gui.AddWidget(std::make_unique<gui::Button>(
                          &_Version->Icons.Button43px,
                          &_Version->Icons.Button43pxPressed,
                          gui::Button::ButtonType::Normal,
                          "Normal",
                          Pixel(0xFF, 0xFF, 0xFF),
                          &_Version->Fonts.InterfaceSmall,
                          []() { std::cout << "Normal button clicked!\n"; }),
                  gui::Position(10, 10));
    Gui.AddWidget(std::make_unique<gui::Button>(
                          &_Version->Icons.Button43px,
                          &_Version->Icons.Button43pxPressed,
                          gui::Button::ButtonType::Toggle,
                          "Toggle",
                          Pixel(0xFF, 0xFF, 0xFF),
                          &_Version->Fonts.InterfaceSmall,
                          []() { std::cout << "Toggle button clicked!\n"; }),
                  gui::Position(10, 50));
    Gui.AddWidget(
            std::make_unique<gui::Window>(
                    200,
                    200,
                    _Version.get(),
                    gui::Window::Type::Sidebar,
                    &_Version->Icons.BattleIcon,
                    "Battle",
                    []() {
                        std::cout << "Battle window close button clicked!\n";
                    }),
            gui::Position(100, 100));
    
    GuiTexture.reset(SDL_CreateTexture(Renderer.get(),
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
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                GuiState._MouseLeftDown = true;
                Gui.MouseLeftDown(gui::Position(event.button.x, event.button.y));
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                GuiState._MouseLeftDown = false;
                Gui.MouseLeftUp(gui::Position(event.button.x, event.button.y));
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
    SDL_GetMouseState(&GuiState._MouseX, &GuiState._MouseY);

    SDL_SetRenderDrawColor(Renderer.get(), 0, 0, 0, 255);
    SDL_RenderClear(Renderer.get());

    // Render gui to texture
    if (SDL_LockTexture(GuiTexture.get(),
                         nullptr,
                         (void **)&Gui.GuiCanvas->Buffer,
                         &Gui.GuiCanvas->Stride) != 0) {
        std::cerr << "Failed to lock texture: " << SDL_GetError() << std::endl;
        exit(1);
    }
    Gui.Render(GuiState);
    SDL_UnlockTexture(GuiTexture.get());

    // Render texture to window
    SDL_RenderCopy(Renderer.get(), GuiTexture.get(), nullptr, nullptr);
    SDL_RenderPresent(Renderer.get());
}


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
        const MemoryFile pictures(dataFolder / "Tibia.pic");
        const MemoryFile sprites(dataFolder / "Tibia.spr");
        const MemoryFile types(dataFolder / "Tibia.dat");
        _Version = std::make_unique<Version>(
                VersionTriplet(major, minor, 0),
                pictures.Reader(),
                sprites.Reader(),
                types.Reader());

    } catch (const ErrorBase &error) {
        std::cerr << "Unrecoverable error (" << error.Description() << ")"
                  << std::endl;
        return 1;
    }

    Window.reset(SDL_CreateWindow("tibiarc GUI test",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  800,
                                  600,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
    if (!Window) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return 1;
    }

    Renderer.reset(SDL_CreateRenderer(Window.get(),
                                      -1,
                                      SDL_RENDERER_ACCELERATED |
                                              SDL_RENDERER_PRESENTVSYNC |
                                              SDL_RENDERER_TARGETTEXTURE));
    if (!Renderer) {
        std::cerr << "Failed to create renderer: " << SDL_GetError()
                  << std::endl;
        return 1;
    }

    if (SDL_SetRenderDrawBlendMode(Renderer.get(), SDL_BLENDMODE_BLEND) != 0) {
        std::cerr << "Failed to set blend mode: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    handle_resize();

    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}