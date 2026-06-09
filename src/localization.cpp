#include "../include/utils.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <zbar.h>

using namespace std;
using namespace cv;
using namespace zbar;

int find_largest_idx(vector<vector<Point>> contours) {
  int idx = 0;
  double largest_area = contourArea(contours[0]);

  for (int i = 1; i < contours.size(); ++i) {
    auto area = contourArea(contours[i]);
    if (area > largest_area) {
      largest_area = area;
      idx = i;
    }
  }
  return idx;
}

pair<int, Image> simple_scan(Mat &gray) {
  Mat copy = gray.clone();
  if (copy.empty()) {
    cerr << "EMPTY IMAGE HERE" << endl;
  }

  if (copy.channels() == 3) {
    cvtColor(copy, copy, COLOR_BGR2GRAY);
  }
  ImageScanner scanner;
  scanner.set_config(ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

  int w = copy.cols, h = copy.rows;
  uchar *raw = (uchar *)copy.data;

  Image image(w, h, "Y800", raw, w * h);

  int n = scanner.scan(image);
  image.set_data(NULL, 0);
  return {n, image};
}

set<pair<string, string>> extract_codes_simple(int n, Image image) {
  set<pair<string, string>> codes;

  for (auto symbol = image.symbol_begin(); symbol != image.symbol_end();
       ++symbol) {
    string type = symbol->get_type_name();
    string data = symbol->get_data();
    if (!codes.count({type, data}))
      cout << "Found [" << type << "]: " << data << endl;

    codes.insert({type, data});
  }

  return codes;
}

/**
 *  @brief returns decoded symbol sequence and does imshow with selected area
 */
set<pair<string, string>> zbar_code_parse(Mat &src) {
  set<pair<string, string>> codes;
  Mat gray;
  cvtColor(src, gray, COLOR_BGR2GRAY);

  auto [n, image] = simple_scan(gray);

  if (n == 0) {
    cout << "No bar codes" << endl;
    return codes;
  }

  for (auto symbol = image.symbol_begin(); symbol != image.symbol_end();
       ++symbol) {
    string type = symbol->get_type_name();
    string data = symbol->get_data();
    if (!codes.count({type, data}))
      cout << "Found [" << type << "]: " << data << endl;

    codes.insert({type, data});

    vector<Point> points;
    for (int i = 0; i < symbol->get_location_size(); i++) {
      points.push_back(
          Point(symbol->get_location_x(i), symbol->get_location_y(i)));
    }

    if (points.size() == 4) {
      for (int i = 0; i < 4; i++) {
        line(src, points[i], points[(i + 1) % 4], Scalar(0, 255, 0), 3);
      }
    } else if (points.size() > 0) {
      polylines(src, points, true, Scalar(0, 255, 0), 3);
    }

    if (!points.empty()) {
      putText(src, data, Point(points[0].x, points[0].y - 10),
              FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
    }
  }
  return codes;
}

set<pair<string, string>> inv_rot_code_parse(Mat &src) {
  set<pair<string, string>> codes;
  Mat gray;
  cvtColor(src, gray, COLOR_BGR2GRAY);

  for (int i = 0; i < 4; ++i) {

    auto [n, image] = simple_scan(gray);

    codes.merge(extract_codes_simple(n, image));

    rotate(gray, gray, ROTATE_90_CLOCKWISE);
  }

  return codes;
}

set<pair<string, string>> bilateral_parse(Mat &src) {
  Mat gray, filtered;
  if (src.channels() == 3) {
    cvtColor(src, gray, COLOR_BGR2GRAY);
  } else {
    gray = src;
  }

  bilateralFilter(gray, filtered, 9, 75, 75);

  auto [n, image] = simple_scan(filtered);
  return extract_codes_simple(n, image);
}

set<pair<string, string>> clahe_parse(Mat &src) {
  Mat gray, enhanced;
  if (src.channels() == 3) {
    cvtColor(src, gray, COLOR_BGR2GRAY);
  } else {
    gray = src;
  }

  Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
  clahe->apply(gray, enhanced);

  auto [n, image] = simple_scan(enhanced);
  return extract_codes_simple(n, image);
}

set<pair<string, string>> clahe_bilateral_parse(Mat &src) {
  Mat gray, enhanced, filtered;
  if (src.channels() == 3) {
    cvtColor(src, gray, COLOR_BGR2GRAY);
  } else {
    gray = src;
  }

  Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
  clahe->apply(gray, enhanced);
  bilateralFilter(enhanced, filtered, 9, 75, 75);

  auto [n, image] = simple_scan(filtered);
  return extract_codes_simple(n, image);
}
