#include "Color.hpp"
#include <iostream>

namespace prog {
    // Default constructor, sets every rgb value to 0
    Color::Color() {
        red_ = 0;
        green_ = 0;
        blue_ = 0;
    }

    // Constructor with a color as parameter, sets the rgb values to the rgb values of other color
    Color::Color(const Color& other) {
        red_ = other.red_;
        blue_ = other.blue_;
        green_ = other.green_;
    }

    // Constructor with three rgb values as parameter
    Color::Color(rgb_value red, rgb_value green, rgb_value blue) {
        red_ = red;
        green_ = green;
        blue_ = blue;
    }

    // Function to get the red value of a color
    rgb_value Color::red() const {
        return red_;
    }
    
    // Function to get the green value of a color
    rgb_value Color::green() const {
        return green_;
    }
    
    // Function to get the blue value of a color
    rgb_value Color::blue() const {
        return blue_;
    }

    // Function to get the red mutable value of a color
    rgb_value& Color::red()  {
        return red_;
    }
    
    // Function to get the green mutable value of a color
    rgb_value& Color::green()  {
      return green_;
    }
    
    // Function to get the blue mutable value of a color
    rgb_value& Color::blue()  {
      return blue_;
    }
}
