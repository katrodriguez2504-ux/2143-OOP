#include "json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

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

// Function to print a shape to console using ASCII characters
void print_shape(const Shape& shape) {
    // Determine min/max bounds in case there are negative coordinates
    int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
    for (const auto& c : shape.cells) {
        min_x = std::min(min_x, c.x);
        max_x = std::max(max_x, c.x);
        min_y = std::min(min_y, c.y);
        max_y = std::max(max_y, c.y);
    }

    int width = max_x - min_x + 1;
    int height = max_y - min_y + 1;

    std::vector<std::string> grid(height, std::string(width, '.'));

    // Plot live cells, adjusted for offset
    for (const auto& c : shape.cells) {
        int gx = c.x - min_x;
        int gy = c.y - min_y;
        if (gy >= 0 && gy < height && gx >= 0 && gx < width) {
            grid[gy][gx] = 'X';
        }
    }

    std::cout << "\n" << shape.name << " (" << shape.width << "x" << shape.height << ")\n";
    for (const auto& row : grid)
        std::cout << row << '\n';
}

int main(int argc, char** argv) {
    std::ifstream file("shapes.json");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open shapes.json\n";
        return 1;
    }

    json data;
    try {
        file >> data;
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return 1;
    }

    if (!data.contains("shapes")) {
        std::cerr << "Error: JSON missing 'shapes' key\n";
        return 1;
    }

    auto shapes_data = data["shapes"];
    std::cout << "Available shapes:\n";
    for (auto it = shapes_data.begin(); it != shapes_data.end(); ++it)
        std::cout << " - " << it.key() << '\n';

    std::cout << "\nTotal shapes loaded: " << shapes_data.size() << "\n";

    std::string choice;
    std::cout << "\nEnter shape name: ";
    std::cin >> choice;

    if (!shapes_data.contains(choice)) {
        std::cerr << "Shape not found.\n";
        return 1;
    }

    auto shape_json = shapes_data[choice];

    Shape shape;
    shape.name = choice;
    shape.width = shape_json["size"]["w"];
    shape.height = shape_json["size"]["h"];

    for (auto& cell : shape_json["cells"]) {
        shape.cells.push_back({cell["x"], cell["y"]});
    }

    print_shape(shape);
    return 0;
}
