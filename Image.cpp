#include "Image.hpp"

namespace prog
{
  // Creates an image with width w and height h and every pixel
  // is set with the color of fill, defined in Image.hpp
  Image::Image(int w, int h, const Color &fill) : width_(w), height_(h), pixels_(h, std::vector<Color>(w, fill))
  { // Implemented in Image.hpp
  }
  Image::~Image()
  { // No need for a destructor
  }

  // Get width function 
  int Image::width() const
  {
    return width_;
  }
  
  // Get height function
  int Image::height() const
  {
    return height_;
  }

  // Get the color of a mutable pixel in the image
  Color& Image::at(int x, int y)
  {
    return pixels_[y][x];
  }
  
  // Get the color of a pixel in the image
  const Color& Image::at(int x, int y) const
  {
    return pixels_[y][x];
  }
}
