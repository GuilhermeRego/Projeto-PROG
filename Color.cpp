#include "Color.hpp"
#include <iostream>

namespace prog {
    // Default constructor
    Color::Color() {
        red_ = 0;
        green_ = 0;
        blue_ = 0;
    }

    // One parameter constructor
    Color::Color(const Color& c) {
        red_ = c.red_;
        blue_ = c.blue_;
        green_ = c.green_;
    }

    // Three parameter constructor
    Color::Color(rgb_value red, rgb_value green, rgb_value blue) {
        red_ = red;
        green_ = green;
        blue_ = blue;
    }

    // Get RGB values
    rgb_value Color::red() const {
        return red_;
    }
    rgb_value Color::green() const {
        return green_;
    }
    rgb_value Color::blue() const {
        return blue_;
    }

    // Get mutable RGB values
    rgb_value& Color::red()  {
        return red_;
    }
    rgb_value& Color::green()  {
      return green_;
    }
    rgb_value& Color::blue()  {
      return blue_;
    }
}
