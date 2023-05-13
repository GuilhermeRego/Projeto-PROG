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
        if (command == "rotate_left") {
            rotate_left();
            continue;
        }
        if (command == "rotate_right") {
            rotate_right();
            continue;
        }
        if (command == "crop") {
            int x, y, w, h;
            input >> x >> y >> w >> h;
            crop(x, y, w, h);
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
        for (int y = 0; y < pixels_.height(); y++) {                    // Loops that run through the image 
            for (int x = 0; x < pixels_.width(); x++) {
                rgb_value r = pixels_(x, y).red();                      
                rgb_value g = pixels_(x, y).green();                    // rgb_values r, g and b keep the red, green and blue values of the pixel in (x,y)
                rgb_value b = pixels_(x, y).blue();
                pixels_(x, y) = Color(255 - r, 255 - g, 255 - b);       // The pixel color is set to the inverse
            }
        }
    }

    void Script::to_gray_scale() {
        // Transforms each pixel (r, g, b) to (r + g + b)/3
        for (int y = 0; y < pixels_.height(); y++) {                    // Loops that run through the image
            for (int x = 0; x < pixels_.width(); x++) {
                rgb_value r = pixels_(x, y).red();
                rgb_value g = pixels_(x, y).green();                    // rgb_values r, g and b keep the red, green and blue values of the pixel in (x,y)
                rgb_value b = pixels_(x, y).blue();
                rgb_value v = (r + g + b) / 3;                          // New rgb_value that will be the color of the pixel (x,y)
                pixels_(x, y) = Color(v, v, v);
            }
        }
    }

    void Script::replace(rgb_value r1, rgb_value g1, rgb_value b1, rgb_value r2, rgb_value g2, rgb_value b2) {
        // Replaces all (r1,  g1, b1) pixels by (r2,  g2, b2)
        for (int y = 0; y < pixels_.height(); y++) {                    // Loops that run through the image
            for (int x = 0; x < pixels_.width(); x++) {
                if (pixels_(x, y) == Color(r1, g1, b1)) {               // Replace only if the pixel (x,y) is (r1, g1, b1)
                    pixels_(x, y) = Color(r2, g2, b2);                  // by (r2, g2, b2), as requested
                }
            }
        }
    }

    void Script::fill(int x, int y, int w, int h, rgb_value r, rgb_value g, rgb_value b) {
        // Assign (r, g, b) to all pixels contained in rectangle defined by top-left corner (x, y)
        for (int j = y; j < y + h; j++) {                               // Loops that run through the image
            for (int i = x; i < x + w; i++) {
                pixels_(i, j) = Color(r, g, b);                         // Every pixel in the image is set to the color (r, g, b)
            }
        }
    }

    void Script::h_mirror() {
        // Mirror image horizontally
        for (int y = 0; y < pixels_.height(); y++) {                    // Loops that run through the image
            for (int x = 0; x < pixels_.width() / 2; x++) {
                Color temp = pixels_(x, y);                             // Set the color of (x,y) to the correspondent pixel mirrored
                pixels_(x, y) = pixels_(pixels_.width() - 1 - x, y);    // that is (pixels_.width() - 1 - x, y) and vice-versa
                pixels_(pixels_.width() - 1 - x, y) = temp;             // the color temp is set so we don't lose the color of the current pixel (x,y)
            }
        }
    }

    void Script::v_mirror() {
        // Mirror image vertically
        for (int y = 0; y < pixels_.height() / 2; y++) {                // Loops that run through the image
            for (int x = 0; x < pixels_.width(); x++) {
                Color temp = pixels_(x, y);                             // Set the color of (x,y) to the correspondent pixel mirrored
                pixels_(x, y) = pixels_(x, pixels_.height() - 1 - y);   // that is (x, pixels_.height() - 1 - y) and vice-versa
                pixels_(x, pixels_.height() - 1 - y) = temp;
            }
        }
    }

    void Script::add(string filename, rgb_value r, rgb_value g, rgb_value b, int x, int y) {
        // Copy all pixels from image stored in PNG file filename to the rectangle of the current image
        PNG png;
        png.loadFromPNG(filename);
        for (int j = 0; j < png.height(); j++) {                        // Loops that run through the png
            for (int i = 0; i < png.width(); i++) {
                Color color = png(i, j);                                // Get the color of the current pixel in the png (i, j)
                if (color != Color(r, g, b)) {                          // If the color is not the same in the png as it is in the image,
                    pixels_(x + i, y + j) = color;                      // we need to replace it, and the correspondent pixel in the png
                }                                                       // is in x + i (reallocating to the x value of the png) and y + j
            }                                                           // (reallocating to the y value of the png)
        }
    }

    void Script::crop(int x, int y, int w, int h){
        // Crop the image
        Image new_image(w, h);                                          // Create the new image which will be the result of the crop
        for (int x_ = 0; x_ < w; x_++){                                 // Loops that will run through the new image, coloring every pixel
            for (int y_ = 0; y_ < h; y_++){
                new_image.at(x_, y_) = pixels_(x + x_, y + y_);         // Copying the pixels in the rectangle of the original image to the new one
            }
        }
        delete image;                                                   // Delete the old image
        image = new Image(new_image);                                   // and setting the new one as the current image
    }

    void Script::rotate_left(){
        // Rotate left
        Image new_image(height_, width_);                               // Since we are rotating the image 90º, the new image will have its 
        for (int x = 0; x < width_; x++){                               // width as the height of the original image and the same to its height
            for (int y = 0; y < height_; y++){                          // Loops that run through the image
                new_image.at(y, width_ - x - 1) = pixels_(x, y);        // Setting the pixels of the new image, its x will be the y of the image and its
            }                                                           // y will be the x of the image starting by the end
        }
        delete image;                                                   // Delete the old image
        image = new Image(new_image);                                   // and setting the new one as the current image
    }

    void Script::rotate_right(){
        // Rotate right
        Image new_image(height_, width_);                               // Since we are rotating the image 90º, the new image will have its
        for (int x = 0; x < width_; x++){                               // width as the height of the original image and the same to its height 
            for (int y = 0; y < height_; y++){                          // Loops that run through the image
                new_image.at(height_ - y - 1, x) = pixels_(x, y);       // Setting the pixels of the new image, its x will be the y of the image starting
            }                                                           // by the end and its y will be the x of the image
        }
        delete image;                                                   // Delete the old image
        image = new Image(new_image);                                   // and setting the new one as the current image
    }
}
