#include "Image.hpp"

namespace prog
{
  //Cria uma imagem com largura w, altura h e todos os pixeis
  //implementados com a cor fill, definido em Image.hpp
  Image::Image(int w, int h, const Color &fill) : width_(w), height_(h), pixels_(h, std::vector<Color>(w, fill))
  {
  }
  Image::~Image()
  {
  }

  //Obter valores de largura e altura
  int Image::width() const
  {
    return width_;
  }
  int Image::height() const
  {
    return height_;
  }

  //Obter cores da imagem num pixel específico mutável
  Color& Image::at(int x, int y)
  {
    return pixels_[y][x];
  }
  //Obter cores da imagem num pixel
  const Color& Image::at(int x, int y) const
  {
    return pixels_[y][x];
  }
}
