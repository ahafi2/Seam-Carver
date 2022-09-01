#include "image_ppm.hpp"

// implement the rest of ImagePPM's functions here

// given functions below, DO NOT MODIFY

ImagePPM::ImagePPM(const std::string& path) {
  std::ifstream input_file(path);
  input_file >> *this;
  input_file.close();
}

ImagePPM::ImagePPM(const ImagePPM& source) { *this = source; }

ImagePPM& ImagePPM::operator=(const ImagePPM& source) {
  if (this == &source) {
    return *this;
  }

  Clear();

  height_ = source.height_;
  width_ = source.width_;
  max_color_value_ = source.max_color_value_;

  pixels_ = new Pixel*[height_];
  for (int row = 0; row < height_; row++) {
    pixels_[row] = new Pixel[width_];

    for (int col = 0; col < width_; col++) {
      pixels_[row][col] = source.pixels_[row][col];
    }
  }

  return *this;
}

ImagePPM::~ImagePPM() { Clear(); }

void ImagePPM::Clear() {
  for (int i = 0; i < height_; i++) {
    delete[] pixels_[i];
  }

  delete[] pixels_;

  height_ = 0;
  width_ = 0;
  pixels_ = nullptr;
}

std::istream& operator>>(std::istream& is, ImagePPM& image) {
  image.Clear();
  std::string line;
  // ignore magic number line
  getline(is, line);
  // check to see if there's a comment line
  getline(is, line);
  if (line[0] == '#') {
    getline(is, line);
  }
  // parse width and height
  int space = line.find_first_of(' ');
  image.width_ = std::stoi(line.substr(0, space));
  image.height_ = std::stoi(line.substr(space + 1));
  // get max color value
  getline(is, line);
  image.max_color_value_ = std::stoi(line);
  // init and fill in Pixels array
  image.pixels_ = new Pixel*[image.height_];
  for (int i = 0; i < image.height_; i++) {
    image.pixels_[i] = new Pixel[image.width_];
  }
  for (int row = 0; row < image.height_; row++) {
    for (int col = 0; col < image.width_; col++) {
      getline(is, line);
      int red = std::stoi(line);
      getline(is, line);
      int green = std::stoi(line);
      getline(is, line);
      int blue = std::stoi(line);

      Pixel p(red, green, blue);
      image.pixels_[row][col] = p;
    }
  }
  return is;
}
std::string ImagePPM::ToString() const {
  std::string result;
  result += "Width " + std::to_string(GetWidth()) + " Height " +
            std::to_string(GetHeight());
  result.push_back('\n');
  result += std::to_string(GetMaxColorValue());
  result.push_back('\n');
  for (int row = 0; row < GetHeight(); ++row) {
    for (int col = 0; col < GetWidth(); ++col) {
      Pixel rgb = GetPixel(row, col);
      result += std::to_string(rgb.GetRed()) + ",";
      result += std::to_string(rgb.GetGreen()) + ",";
      result += std::to_string(rgb.GetBlue()) + ",";
      result.push_back(' ');
    }
    result.push_back('\n');
  }

  return result;
}
std::ostream& operator<<(std::ostream& os, const ImagePPM& image) {
  os << "P3" << '\n';
  os << std::to_string(image.GetWidth()) + " " +
            std::to_string(image.GetHeight())
     << '\n';
  os << std::to_string(image.GetMaxColorValue()) << std::endl;
  for (int row = 0; row < image.GetHeight(); ++row) {
    for (int col = 0; col < image.GetWidth(); ++col) {
      Pixel rgb = image.GetPixel(row, col);
      os << std::to_string(rgb.GetRed()) << '\n';
      os << std::to_string(rgb.GetGreen()) << '\n';
      os << std::to_string(rgb.GetBlue()) << '\n';
      // I don't know if I need this
    }
  }

  return os;
}
// done
Pixel ImagePPM::GetPixel(int row, int col) const { return pixels_[row][col]; }
// done
int ImagePPM::GetMaxColorValue() const { return max_color_value_; }
void ImagePPM::RVS(const int* skips) {
  // make a 2d array that's one width smaller
  auto* carved = new Pixel*[height_];
  for (int i = 0; i < height_; ++i) {
    carved[i] = new Pixel[width_ - 1];
  }
  // loop thrugh row first
  // loop through coloumn second
  for (int row = 0; row < height_; ++row) {
    int offset = 0;
    for (int col = 0; col < width_ - 1; ++col) {
      if (skips[row] == col) {
        offset++;
      }

      carved[row][col] = GetPixel(row, col + offset);
    }
  }
  // deallocate pixels
  for (int i = 0; i < height_; ++i) {
    delete pixels_[i];
  }
  delete[] pixels_;
  pixels_ = carved;
  width_--;
}
void ImagePPM::RHS(const int* skips) {
  // make a 2d array that's one height smaller
  auto** carved = new Pixel*[height_ - 1];
  for (int i = 0; i < height_ - 1; ++i) {
    carved[i] = new Pixel[width_];
  }
  // loop through coloumns first
  // you go down the coloumn, changin rows, then switch your coloumn
  for (int col = 0; col < width_; ++col) {
    int offset = 0;
    for (int row = 0; row < height_ - 1; ++row) {
      if (skips[col] == row) {
        offset++;
      }
      carved[row][col] = GetPixel(row + offset, col);
    }
  }
  // deallocate pixels
  for (int i = 0; i < height_; ++i) {
    delete pixels_[i];
  }
  delete[] pixels_;
  pixels_ = carved;
  height_--;
}