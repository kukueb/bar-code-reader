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
  int    idx = 0;
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

/**
 *  @brief returns decoded symbol sequence and does imshow with selected area
 */
set<pair<string, string>> zbar_code_parse(Mat &src) {
  ImageScanner scanner;
  scanner.set_config(ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);

  Mat gray;
  cvtColor(src, gray, COLOR_BGR2GRAY);
  int    w = gray.cols, h = gray.rows;
  uchar *raw = (uchar *)gray.data;

  Image image(w, h, "Y800", raw, w * h);

  int n = scanner.scan(image);

  set<pair<string, string>> codes;

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
