#include <iostream>
#include <fstream>
#include "Script.hpp"
#include "PNG.hpp"
#include "XPM2.hpp"

using namespace std;

namespace prog {
    // Use to read color values from a script file.
    istream& operator>>(istream& input, Color& c) {
        int r, g, b;
        input >> r >> g >> b;
        c.red() = r;
        c.green() = g;
        c.blue() = b;
        return input;
    }

    Script::Script(const string& filename) :
            image(nullptr), input(filename) {

    }
    void Script::clear_image_if_any() {
        if (image != nullptr) {
            delete image;
            image = nullptr;
        }
    }
    Script::~Script() {
        clear_image_if_any();
    }

    void Script::run() {
    string command;
    while (input >> command) {
        cout << "Executing command '" << command << "' ..." << endl;
        if (command == "open") {
            open(); // Load an image from file
            continue;
        }
        if (command == "blank") {
            blank(); // Create a new blank image
            continue;
        }
        // Other commands require an image to be previously loaded.
        if (!pixels_.is_loaded()) {
            cout << "Error: No image is currently loaded." << endl;
            continue;
        }
        if (command == "save") {
            save(); // Save the current image to file
            continue;
        } 
        if (command == "invert") {
            pixels_.invert(); // Invert the colors of the current image
            continue;
        }
        if (command == "to_gray_scale") {
            pixels_.to_gray_scale(); // Convert the current image to grayscale
            continue;
        }
        if (command == "replace") {
            int r1, g1, b1, r2, g2, b2;
            input >> r1 >> g1 >> b1 >> r2 >> g2 >> b2;
            pixels_.replace(r1, g1, b1, r2, g2, b2); // Replace one color with another in the current image
            continue;
        }
        if (command == "fill") {
            int x, y, w, h, r, g, b;
            input >> x >> y >> w >> h >> r >> g >> b;
            pixels_.fill(x, y, w, h, r, g, b); // Fill a rectangle with a given color in the current image
            continue;
        }
        if (command == "h_mirror") {
            pixels_.h_mirror(); // Mirror the current image horizontally
            continue;
        }
        if (command == "v_mirror") {
            pixels_.v_mirror(); // Mirror the current image vertically
            continue;
        }
        if (command == "add") {
            string filename;
            int r, g, b, x, y;
            input >> filename >> r >> g >> b >> x >> y;
            PNG new_pixels_;
            if (new_pixels_.loadFromPNG(filename)) {
                pixels_.add(new_pixels_, r, g, b, x, y);  // Copy an image from file into the current image at a given position, excluding a specific color
            } else {
                cout << "Error: Could not load image from file '" << filename << "'." << endl;
            }
            continue;
        }
        cout << "Error: Unrecognized command '" << command << "'." << endl;
    }
}

    void Script::open() {
        // Replace current image (if any) with image read from PNG file.
        clear_image_if_any();
        string filename;
        input >> filename;
        image = loadFromPNG(filename);
    }
    void Script::blank() {
        // Replace current image (if any) with blank image.
        clear_image_if_any();
        int w, h;
        Color fill;
        input >> w >> h >> fill;
        image = new Image(w, h, fill);
    }
    void Script::save() {
        // Save current image to PNG file.
        string filename;
        input >> filename;
        saveToPNG(filename, image);
    }
    
void Script::invert() {
    // Transforms each individual pixel (r, g, b) to (255-r,255-g,255-b)
    for (int y = 0; y < pixels_.height(); y++) {
        for (int x = 0; x < pixels_.width(); x++) {
            rgb_value r = pixels_(x, y).red();
            rgb_value g = pixels_(x, y).green();
            rgb_value b = pixels_(x, y).blue();
            pixels_(x, y) = Color(255 - r, 255 - g, 255 - b);
        }
    }
}

void Script::to_gray_scale() {
    // Transforms each pixel (r, g, b) to (r + g + b)/3
    for (int y = 0; y < pixels_.height(); y++) {
        for (int x = 0; x < pixels_.width(); x++) {
            rgb_value r = pixels_(x, y).red();
            rgb_value g = pixels_(x, y).green();
            rgb_value b = pixels_(x, y).blue();
            rgb_value v = (r + g + b) / 3;
            pixels_(x, y) = Color(v, v, v);
        }
    }
}

void Script::replace(rgb_value r1, rgb_value g1, rgb_value b1, rgb_value r2, rgb_value g2, rgb_value b2) {
    // Replaces all (r1,  g1, b1) pixels by (r2,  g2, b2)
    for (int y = 0; y < pixels_.height(); y++) {
        for (int x = 0; x < pixels_.width(); x++) {
            if (pixels_(x, y) == Color(r1, g1, b1)) {
                pixels_(x, y) = Color(r2, g2, b2);
            }
        }
    }
}

void Script::fill(int x, int y, int w, int h, rgb_value r, rgb_value g, rgb_value b) {
    // Assign (r, g, b) to all pixels contained in rectangle defined by top-left corner (x, y)
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            pixels_(i, j) = Color(r, g, b);
        }
    }
}

void Script::h_mirror() {
    // Mirror image horizontally
    for (int y = 0; y < pixels_.height(); y++) {
        for (int x = 0; x < pixels_.width() / 2; x++) {
            Color temp = pixels_(x, y);
            pixels_(x, y) = pixels_(pixels_.width() - 1 - x, y);
            pixels_(pixels_.width() - 1 - x, y) = temp;
        }
    }
}

void Script::v_mirror() {
    // Mirror image vertically
    for (int y = 0; y < pixels_.height() / 2; y++) {
        for (int x = 0; x < pixels_.width(); x++) {
            Color temp = pixels_(x, y);
            pixels_(x, y) = pixels_(x, pixels_.height() - 1 - y);
            pixels_(x, pixels_.height() - 1 - y) = temp;
        }
    }
}

void Script::add(string filename, rgb_value r, rgb_value g, rgb_value b, int x, int y) {
    // Copy all pixels from image stored in PNG file filename to the rectangle of the current image
    PNG png;
    png.loadFromPNG(filename);
    for (int j = 0; j < png.height(); j++) {
        for (int i = 0; i < png.width(); i++) {
            Color color = png(i, j);
            if (color != Color(r, g, b)) {
                pixels_(x + i, y + j) = color;
            }
        }
    }
}

}
