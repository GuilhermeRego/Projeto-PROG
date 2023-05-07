#include "Color.hpp"
#include <iostream>

namespace prog {
    //Construtor default
    Color::Color() {
        red_ = 0;
        green_ = 0;
        blue_ = 0;
    }

    //Construtor usando um argumento
    Color::Color(const Color& other) {
        red_ = other.red_;
        blue_ = other.blue_;
        green_ = other.green_;
    }

    //Construtor usando três argumentos
    Color::Color(rgb_value red, rgb_value green, rgb_value blue) {
        red_ = red;
        green_ = green;
        blue_ = blue;
    }

    //Obter valores RGB
    rgb_value Color::red() const {
        return red_;
    }
    rgb_value Color::green() const {
        return green_;
    }
    rgb_value Color::blue() const {
        return blue_;
    }

    //Obter valores valores RGB mutáveis
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
