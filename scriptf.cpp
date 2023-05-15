#include "PNG.hpp"
#include <iostream>
#include <fstream>
#include "Script.hpp"

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
                open();
                continue;
            }
            if (command == "blank") {
                blank();
                continue;
            }
            // Other commands require an image to be previously loaded.
            if (image == nullptr) {
                cout << "Error: No image loaded" << endl;
                continue;
            }
            if (command == "save") {
                save();
                continue;
            } 
            if (command == "invert") {
                invert();
                continue;
            }
            if (command == "to_gray_scale") {
                to_gray_scale();
                continue;
            }
            if (command == "replace") {
                int r1, g1, b1, r2, g2, b2;
                input >> r1 >> g1 >> b1 >> r2 >> g2 >> b2;
                replace(r1, g1, b1,r2, g2, b2);
                continue;
            }
            if (command == "fill") {
                int x, y, w, h;
                int r, g, b;
                input >> x >> y >> w >> h >> r >> g >> b;
                fill(x, y, w, h, r, g, b);
                continue;
            }
            if (command == "h_mirror") {
                h_mirror();
                continue;
            }
            if (command == "v_mirror") {
                v_mirror();
                continue;
            }
            if (command == "add") {
                string filename;
                int x, y;
                int r, g, b;
                input >> filename >> r >> g >> b >> x >> y;
                add(filename,r , g , b , x, y);
                continue;
            }
            if (command == "crop") {
                int x, y, w, h;
                input >> x >> y >> w >> h;
                crop(x, y, w, h);
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
        for (int y = 0; y < image->height(); ++y) {
        for (int x = 0; x < image->width(); ++x) {
            Color& c = image->at(x, y);
            c.red() = 255 - c.red();
            c.green() = 255 - c.green();
            c.blue() = 255 - c.blue();
        }
    }
    }

    void Script::to_gray_scale() {

    for (int i = 0; i < image->width(); i++) {
        for (int j = 0; j < image->height(); j++) {
            Color& c = image->at(i, j);
            rgb_value gray = (c.red() + c.green() + c.blue()) / 3;
            c.red() = gray;
            c.green() = gray;
            c.blue() = gray;
        }
    }
}

   void Script::replace(rgb_value r1, rgb_value g1, rgb_value b1, rgb_value r2, rgb_value g2, rgb_value b2) {
    for (int y = 0; y < image->height(); y++) {
        for (int x = 0; x < image->width(); x++) {
            Color& pixel = image->at(x, y);
            if ((pixel.red() == r1) && (pixel.green() == g1) && (pixel.blue() == b1)) {
                pixel.red() = r2;
                pixel.green() = g2;
                pixel.blue() = b2;
            }
        }
    }
}

    void Script::fill(int x, int y, int w, int h, rgb_value r, rgb_value g, rgb_value b) {

    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            Color& pixel = image->at(i, j);
            pixel.red() = r;
            pixel.green() = g;
            pixel.blue() = b;
        }
    }
}

    void Script::h_mirror() {
    for (int y = 0; y < image->height(); ++y) {
        for (int x = 0; x < image->width() / 2; ++x) {
            std::swap(image->at(x, y), image->at(image->width() - 1 - x, y));
        }
    }
}

void Script::v_mirror() {
    for (int y = 0; y < image->height() / 2; ++y) {
        for (int x = 0; x < image->width(); ++x) {
            std::swap(image->at(x, y), image->at(x, image->height() - 1 - y));
        }
    }
}

void Script::add(string filename, rgb_value r, rgb_value g, rgb_value b, int x, int y) {
    // Load the image from the file
    Image* png = loadFromPNG(filename);
    if (!png) {
        // Failed to load image, return
        return;
    }
    // Replace pixels in the region with pixels from the loaded image
    for (int j = 0; j < png->height(); j++) {
        for (int i = 0; i < png->width(); i++) {
            Color color = png->at(i, j);
            if (color != Color(r, g, b)) {
                image->at(x + i, y + j) = color;
            }
        }
    }
    
    // Free the loaded image
    delete png;
}


    void Script::crop(int x, int y, int w, int h){
        // Crop the image
        Image new_image(w, h);                                          // Create the new image which will be the result of the crop
        for (int x_ = 0; x_ < w; x_++){                                 // Loops that will run through the new image, coloring every pixel
            for (int y_ = 0; y_ < h; y_++){
                new_image.at(x_, y_) = image->at(x + x_, y + y_);         // Copying the pixels in the rectangle of the original image to the new one
            }
        }
        delete image;                                                   // Delete the old image
        image = new Image(new_image);                                   // and setting the new one as the current image
    }

    void Script::rotate_left(){
        // Rotate left
        Image new_image(image->height(), image->width());                               // Since we are rotating the image 90º, the new image will have its 
        for (int x = 0; x < image->width(); x++){                               // width as the height of the original image and the same to its height
            for (int y = 0; y < image->height(); y++){                          // Loops that run through the image
                new_image.at(y, image->width() - x - 1) = image->at(x, y);        // Setting the pixels of the new image, its x will be the y of the image and its
            }                                                           // y will be the x of the image starting by the end
        }
        delete image;                                                   // Delete the old image
        image = new Image(new_image);                                   // and setting the new one as the current image
    }

    void Script::rotate_right(){
        // Rotate right
        Image new_image(image->height(), image->width());                               // Since we are rotating the image 90º, the new image will have its
        for (int x = 0; x < image->width(); x++){                               // width as the height of the original image and the same to its height 
            for (int y = 0; y < image->height(); y++){                          // Loops that run through the image
                new_image.at(image->height() - y - 1, x) = image->at(x, y);       // Setting the pixels of the new image, its x will be the y of the image starting
            }                                                           // by the end and its y will be the x of the image
        }
        delete image;                                                   // Delete the old image
        image = new Image(new_image);                                   // and setting the new one as the current image
    }
}


add mal
