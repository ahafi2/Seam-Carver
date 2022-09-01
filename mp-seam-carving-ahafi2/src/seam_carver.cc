#include "seam_carver.hpp"
// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}
const ImagePPM& SeamCarver::GetImage() const { return image_; }
int SeamCarver::GetHeight() const { return height_; }
int SeamCarver::GetWidth() const { return width_; }
int SeamCarver::GetEnergy(int row, int col) const {
  Pixel left = image_.GetPixel(0, 0);

  if (col == 0) {
    left = image_.GetPixel(row, GetWidth() - 1);
  } else {
    left = image_.GetPixel(row, col - 1);
  }

  Pixel right = image_.GetPixel(0, 0);
  if (col == GetWidth() - 1) {
    right = image_.GetPixel(row, 0);
  } else {
    right = image_.GetPixel(row, col + 1);
  }

  Pixel up = image_.GetPixel(0, 0);
  if (row == 0) {
    up = image_.GetPixel(GetHeight() - 1, col);
  } else {
    up = image_.GetPixel(row - 1, col);
  }

  Pixel down = image_.GetPixel(0, 0);
  if (row == GetHeight() - 1) {
    down = image_.GetPixel(0, col);
  } else {
    down = image_.GetPixel(row + 1, col);
  }
  return Delta(left, right) + Delta(up, down);
}
int SeamCarver::Delta(Pixel one, Pixel two) const {
  return (one.GetBlue() - two.GetBlue()) * (one.GetBlue() - two.GetBlue()) +
         (one.GetGreen() - two.GetGreen()) * (one.GetGreen() - two.GetGreen()) +
         (one.GetRed() - two.GetRed()) * (one.GetRed() - two.GetRed());
}

// GetEnergy Done (Calls helper funtion Delta)
std::string SeamCarver::ToStringIntTable(int** table) const {
  std::string result;
  result += "Energies combined with least neighbor increment";
  result.push_back('\n');
  for (int row = 0; row < height_; ++row) {
    for (int col = 0; col < width_; ++col) {
      result += std::to_string(table[row][col]) + "  ";
    }
    result.push_back('\n');
  }
  return result;
}
int* SeamCarver::GetVerticalSeam() const {
  int** values = new int*[height_];
  for (int i = 0; i < height_; ++i) {
    values[i] = new int[width_];
  }
  for (int j = 0; j < width_; ++j) {
    values[height_ - 1][j] = GetEnergy(height_ - 1, j);
  }
  for (int row = height_ - 2; row >= 0; --row) {
    for (int col = 0; col < width_; ++col) {
      int down_m = values[row + 1][col];
      if (col == 0) {
        int down_r = values[row + 1][col + 1];
        if (down_r < down_m) {
          values[row][col] = GetEnergy(row, col) + down_r;
        } else {
          values[row][col] = GetEnergy(row, col) + down_m;
        }
      } else if (col == width_ - 1) {
        int down_l = values[row + 1][col - 1];
        if (down_l < down_m) {
          values[row][col] = GetEnergy(row, col) + down_l;
        } else {
          values[row][col] = GetEnergy(row, col) + down_m;
        }
      } else {
        int down_r = values[row + 1][col + 1];
        int down_l = values[row + 1][col - 1];
        if (down_r < down_m && down_r < down_l) {
          values[row][col] = GetEnergy(row, col) + down_r;
        } else if (down_l < down_m && down_l <= down_r) {
          values[row][col] = GetEnergy(row, col) + down_l;
        } else {values[row][col] = GetEnergy(row, col) + down_m;}
      }
    }
  }
  int* result = GetOneVH(values);
  Deallocate(values);
  return result;
}
int* SeamCarver::GetOneDH(
    int** table) const {  
  int* result = new int[width_];
  int min_val = table[0][0];
  int min_row = 0;
  for (int i = 0; i < height_; ++i) {
    if (min_val > table[i][0]) {
      min_val = table[i][0];
      min_row = i;
    }
  }
  result[0] = min_row;
  for (int col = 0; col < width_ - 1; ++col) {
    int mid_r = table[min_row][col + 1];
    if (min_row == 0) {  // top
      int down_r = table[min_row + 1][col + 1];
      if (down_r < mid_r) {
        min_row++;
      }
    } else if (min_row == height_ - 1) {  // bottom
      int up_r = table[min_row - 1][col + 1];
      if (up_r < mid_r) {
        min_row--;
      }
    } else {  // middles
      int up_r = table[min_row - 1][col + 1];
      int down_r = table[min_row + 1][col + 1];
      if (down_r < mid_r && down_r < up_r) {
        min_row++;
      } else if (up_r < mid_r && up_r <= down_r) {
        min_row--;
      }
    }
    result[col + 1] = min_row;
  }
  return result;
}
// clang tidy decrease lines
int* SeamCarver::GetOneVH(int** table) const {
  // std::cout << ToStringIntTable(table);
  int* result = new int[height_];
  int min_val = table[0][0];
  int min_col = 0;
  for (int col = 0; col < width_; ++col) {
    if (min_val > table[0][col]) {
      min_val = table[0][col];
      min_col = col;
    }
  }
  result[0] = min_col;
  for (int row = 0; row < height_ - 1; ++row) {
    if (min_col == 0) {
      // There only exists a down_m and down_r
      int down_m = table[row + 1][min_col];
      int down_r = table[row + 1][min_col + 1];
      if (down_r < down_m) {
        min_col++;
      }
    } else if (min_col == width_ - 1) {
      // there only exists a down _m and down_l
      int down_m = table[row + 1][min_col];
      int down_l = table[row + 1][min_col - 1];
      if (down_l < down_m) {
        min_col--;
      }
    } else {
      int down_m = table[row + 1][min_col];
      int down_r = table[row + 1][min_col + 1];
      int down_l = table[row + 1][min_col - 1];
      if (down_r < down_m && down_r < down_l) {
        min_col++;
      } else if (down_l < down_m && down_l <= down_r) {
        min_col--;
      }
    }
    result[row + 1] = min_col;
  }
  return result;
}
/*
  Vertical
  findMinPathDynamicProgramming():
    Values ← 2D array with height rows and width columns

    For each cell in the last row
        Values[cell_row][cell_col] ← E(cell_row, cell_col)

    For each row starting from the second-to-last row (height - 2) to the first
      For each cell in the row
        Best ← min(Values[down-right], Values[down], Values[down-left])
        Values[cell_row][cell_col] ← Best + E(cell_row, cell_col)

    Return minimum cell of Values[0]
  */
int* SeamCarver::GetHorizontalSeam() const {
  int** values = new int*[height_];
  for (int i = 0; i < height_; ++i) {
    values[i] = new int[width_];
  }
  for (int i = 0; i < height_; ++i) {
    values[i][width_ - 1] = GetEnergy(i, width_ - 1);
  }
  for (int col = width_ - 2; col >= 0; --col) {
    for (int row = 0; row < height_; ++row) {
      int mid_r = values[row][col + 1];
      if (row == 0) {  // top
        int down_r = values[row + 1][col + 1];
        if (down_r < mid_r) {
          values[row][col] = GetEnergy(row, col) + down_r;
        } else {
          values[row][col] = GetEnergy(row, col) + mid_r;
        }
      } else if (row == height_ - 1) {  // bottom
        int up_r = values[row - 1][col + 1];
        if (up_r < mid_r) {
          values[row][col] = GetEnergy(row, col) + up_r;
        } else {
          values[row][col] = GetEnergy(row, col) + mid_r;
        }
      } else {
        int up_r = values[row - 1][col + 1];
        int down_r = values[row + 1][col + 1];
        if (down_r < mid_r && down_r < up_r) {
          values[row][col] = GetEnergy(row, col) + down_r;
        } else if (up_r < mid_r && up_r <= down_r) {
          values[row][col] = GetEnergy(row, col) + up_r;
        } else {values[row][col] = GetEnergy(row, col) + mid_r;}
      }
    }
  }
  int* result = GetOneDH(values);
  Deallocate(values);
  return result;
}
void SeamCarver::Deallocate(int** table) const {
  for (int row = 0; row < height_; ++row) {
    delete[] table[row];
  }
  delete[] table;
}
void SeamCarver::RemoveHorizontalSeam() {
  // basically delete the 2d array with colors, make a whole new one, delete the
  // pixel 2d array and then set it equal to the modified one this is
  // horizontal, so the new image has height - 1 we have to iterate through the
  // rows and traverse through the the 2d pixels and the 1d array at the same
  // time, skip copy when row = result[i]
  // I think you traverse coloumn first and then row second
  // std::cout << "Original File" << std::endl;
  // std::cout << image_.ToString();
  int* skip = GetHorizontalSeam();
  // std::cout << "Seam Path(row indexes)" << std::endl;
  // for (int i = 0; i < width_; ++i) {
  //   std::cout << skip[i] << "  ";
  // }
  // std::cout << "" << std::endl;
  // std::cout << "New Image Data" << std::endl;
  image_.RHS(skip);
  // std::cout << image_.ToString();
  delete[] skip;
  height_--;
}

void SeamCarver::RemoveVerticalSeam() {
  // std::cout << "Original File" << std::endl;
  // std::cout << image_.ToString();
  int* skip = GetVerticalSeam();
  // std::cout << "Seam Path,(coloumn indexes)" << std::endl;
  // for (int i = 0; i < height_; ++i) {
  //   std::cout << skip[i] << "  " << std::endl;
  // }
  // std::cout << "New Image Data" << std::endl;
  image_.RVS(skip);

  // std::cout << image_.ToString();
  // update seam carver parameters, deallocate skip
  delete[] skip;
  width_--;
}