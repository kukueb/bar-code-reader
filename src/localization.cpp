#include "../include/utils.hpp"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <zbar.h>

using namespace std;
using namespace cv;
using namespace zbar;

struct BarcodeResult {
  string type;
  string data;
  vector<Point> points;
};

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

vector<BarcodeResult> scan_image(Mat &gray) {
  vector<BarcodeResult> results;
  Mat copy;

  if (gray.empty()) {
    cerr << "EMPTY IMAGE HERE" << endl;
    return results;
  }

  if (gray.channels() == 3) {
    cvtColor(gray, copy, COLOR_BGR2GRAY);
  } else {
    copy = gray.clone();
  }

  ImageScanner scanner;
  scanner.set_config(ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

  int w = copy.cols, h = copy.rows;
  uchar *raw = (uchar *)copy.data;

  Image image(w, h, "Y800", raw, w * h);
  int n = scanner.scan(image);

  for (auto symbol = image.symbol_begin(); symbol != image.symbol_end();
       ++symbol) {
    BarcodeResult bc;
    bc.type = symbol->get_type_name();
    bc.data = symbol->get_data();

    for (int i = 0; i < symbol->get_location_size(); i++) {
      bc.points.push_back(
          Point(symbol->get_location_x(i), symbol->get_location_y(i)));
    }
    results.push_back(bc);
  }

  image.set_data(NULL, 0);
  return results;
}

set<pair<string, string>>
extract_codes_simple(const vector<BarcodeResult> &barcodes) {
  set<pair<string, string>> codes;

  for (const auto &bc : barcodes) {
    if (!codes.count({bc.type, bc.data})) {
      cout << "Found [" << bc.type << "]: " << bc.data << endl;
    }
    codes.insert({bc.type, bc.data});
  }
  return codes;
}

/**
 * @brief returns decoded symbol sequence and does imshow with selected area
 */
set<pair<string, string>> zbar_code_parse(Mat &src) {
  set<pair<string, string>> codes;
  Mat gray;

  if (src.channels() == 3) {
    cvtColor(src, gray, COLOR_BGR2GRAY);
  } else {
    gray = src;
  }

  auto barcodes = scan_image(gray);

  if (barcodes.empty()) {
    cout << "No bar codes" << endl;
    return codes;
  }

  for (const auto &bc : barcodes) {
    if (!codes.count({bc.type, bc.data}))
      cout << "Found [" << bc.type << "]: " << bc.data << endl;

    codes.insert({bc.type, bc.data});

    if (bc.points.size() == 4) {
      for (int i = 0; i < 4; i++) {
        line(src, bc.points[i], bc.points[(i + 1) % 4], Scalar(0, 255, 0), 3);
      }
    } else if (bc.points.size() > 0) {
      polylines(src, bc.points, true, Scalar(0, 255, 0), 3);
    }

    if (!bc.points.empty()) {
      putText(src, bc.data, Point(bc.points[0].x, bc.points[0].y - 10),
              FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
    }
  }
  return codes;
}

set<pair<string, string>> inv_rot_code_parse(Mat &src) {
  set<pair<string, string>> codes;
  Mat gray;
  if (src.channels() == 3) {
    cvtColor(src, gray, COLOR_BGR2GRAY);
  } else {
    gray = src.clone();
  }

  for (int i = 0; i < 4; ++i) {
    auto barcodes = scan_image(gray);
    codes.merge(extract_codes_simple(barcodes));
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

  auto barcodes = scan_image(filtered);
  return extract_codes_simple(barcodes);
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

  auto barcodes = scan_image(enhanced);
  return extract_codes_simple(barcodes);
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

  // while (true) {
  //   imshow("sat", filtered);
  //   auto key = waitKey(0);
  //   if (key == 'q') {
  //     break;
  //   }
  // }

  auto barcodes = scan_image(filtered);
  return extract_codes_simple(barcodes);
}

set<pair<string, string>> parse_by_saturation(Mat &src) {
  Mat sat;
  if (src.channels() == 3) {
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    vector<Mat> hsv_channels;
    split(hsv, hsv_channels);
    sat = hsv_channels[1];
  } else {
    sat = src;
  }

  // while (true) {
  //   imshow("sat", sat);
  //   auto key = waitKey(0);
  //   if (key == 'q') {
  //     break;
  //   }
  // }

  auto barcodes = scan_image(sat);
  return extract_codes_simple(barcodes);
}
