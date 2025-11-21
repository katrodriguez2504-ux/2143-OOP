#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#include "includes/json.hpp"

using json = nlohmann::json;

struct Cell {
    int x;
    int y;
};

struct Shape {
    std::string name;
    int width;
    int height;
    std::vector<Cell> cells;
};

int main(int argc, char** argv) {
    // ----------------------------
    // Load JSON patterns
    // ----------------------------
    std::ifstream file("patterns.json");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open patterns.json\n";
        return 1;
    }

    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return 1;
    }

    // Support either "shapes" or "patterns" as top-level key
    json patterns_data;
    if (data.contains("shapes")) {
        patterns_data = data["shapes"];
    } else if (data.contains("patterns")) {
        patterns_data = data["patterns"];
    } else {
        std::cerr << "Error: JSON missing 'shapes' or 'patterns' key\n";
        return 1;
    }

    std::cout << "Available patterns:\n";
    for (auto it = patterns_data.begin(); it != patterns_data.end(); ++it) {
        std::cout << " - " << it.key() << '\n';
    }
    std::cout << "\nTotal patterns loaded: " << patterns_data.size() << "\n";

    // Pattern choice: command line arg or stdin
    std::string choice;
    if (argc > 1) {
        choice = argv[1];
        std::cout << "\nUsing pattern from command line: " << choice << "\n";
    } else {
        std::cout << "\nEnter pattern name: ";
        std::cin >> choice;
    }

    if (!patterns_data.contains(choice)) {
        std::cerr << "Pattern not found.\n";
        return 1;
    }

    auto pattern_json = patterns_data[choice];

    Shape shape;
    shape.name = choice;
    shape.width  = pattern_json["size"]["w"];
    shape.height = pattern_json["size"]["h"];

    for (auto& cell : pattern_json["cells"]) {
        shape.cells.push_back({cell["x"], cell["y"]});
    }

    // Compute bounding box of cells (in case coords aren't 0-based)
    int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    for (const auto& c : shape.cells) {
        min_x = std::min(min_x, c.x);
        max_x = std::max(max_x, c.x);
        min_y = std::min(min_y, c.y);
        max_y = std::max(max_y, c.y);
    }
    int patternCellWidth  = max_x - min_x + 1;
    int patternCellHeight = max_y - min_y + 1;

    // ----------------------------
    // SDL configuration
    // ----------------------------
    const int cellSize = 20;
    const int gridWidth = 30;
    const int gridHeight = 30;
    const int windowWidth = cellSize * gridWidth;
    const int windowHeight = cellSize * gridHeight;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Program_02 - SDL Pattern Render",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // ----------------------------
    // Random color for the pattern
    // ----------------------------
    std::mt19937 rng(static_cast<unsigned>(SDL_GetTicks()));
    std::uniform_int_distribution<int> dist(80, 255); // avoid too-dark colors
    Uint8 r = static_cast<Uint8>(dist(rng));
    Uint8 g = static_cast<Uint8>(dist(rng));
    Uint8 b = static_cast<Uint8>(dist(rng));

    // ----------------------------
    // Centering in logical grid
    // ----------------------------
    int offsetCellsX = (gridWidth  - patternCellWidth)  / 2;
    int offsetCellsY = (gridHeight - patternCellHeight) / 2;

    bool running = true;
    SDL_Event event;

    while (running) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false; // ESC to quit
                }
            }
        }

        // Clear background
        SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
        SDL_RenderClear(renderer);

        // Draw pattern cells as solid rectangles
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);

        for (const auto& c : shape.cells) {
            int gx = c.x - min_x + offsetCellsX;  // shifted to start at 0
            int gy = c.y - min_y + offsetCellsY;

            int px = gx * cellSize;
            int py = gy * cellSize;

            SDL_Rect rect { px, py, cellSize, cellSize };
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
