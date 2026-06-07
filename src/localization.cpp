#include "../include/utils.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

vector<Point> find_largest(vector<vector<Point>> contours) {
  vector<Point> largest_contour = contours[0];
  double        largest_area = contourArea(contours[0]);

  for (auto i : contours) {
    auto area = contourArea(i);
    if (area > largest_area) {
      largest_area = area;
      largest_contour = i;
    }
  }
  return largest_contour;
}

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

Mat localize_bar_code_and_straght(const Mat &src) {
  Mat img = src.clone();
  Mat gradient;

  cvtColor(img, img, COLOR_BGR2GRAY);
  GaussianBlur(img, img, cv::Size(5, 5), 0);

  Mat dx, dy;
  Sobel(img, dx, CV_32F, 1, 0, -1);
  Sobel(img, dy, CV_32F, 0, 1, -1);

  Mat abs_dx, abs_dy;
  convertScaleAbs(dx, abs_dx);
  convertScaleAbs(dy, abs_dy);
  subtract(abs_dx, abs_dy, gradient);

  GaussianBlur(gradient, gradient, Size(5, 5), 0);

  Mat binary;
  threshold(gradient, binary, 0, 255, THRESH_BINARY | cv::THRESH_OTSU);

  Mat kernel = getStructuringElement(MORPH_RECT, Size(21, 7));
  morphologyEx(binary, binary, MORPH_CLOSE, kernel);

  erode(binary, binary, Mat(), Point(-1, -1), 4);
  dilate(binary, binary, Mat(), Point(-1, -1), 4);

  vector<vector<Point>> contours;
  findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

  if (contours.empty()) {
    cerr << "Found no contours" << endl;
    return Mat();
  }

  int           largest_contour_idx = find_largest_idx(contours);
  vector<Point> largest_contour = contours[largest_contour_idx];

  // DEBUG TODO: DELETE THIS SHIT

  Mat contoured;
  cvtColor(binary, contoured, COLOR_GRAY2BGR);
  drawContours(contoured, contours, largest_contour_idx, Scalar(0, 255, 0), 2);
  imshow("contoured", contoured);

  // END OF DEBUG TODO: DELETE THIS SHIT

  RotatedRect minRect = minAreaRect(largest_contour);

  vector<Point2f> edge_points(4), ordered_points;
  minRect.points(edge_points);
  ordered_points = order_points(edge_points);

  float width = minRect.size.width;
  float height = minRect.size.height;

  if (width < height) {
    swap(width, height);
  }

  // extrude a little
  int padW = static_cast<int>(width * 0.05f);
  int padH = static_cast<int>(height * 0.05f);

  int dstW = static_cast<int>(width) + (padW * 2);
  int dstH = static_cast<int>(height) + (padH * 2);

  vector<Point2f> destination_points = {
      Point2f(padW, padH), Point2f(dstW - 1 - padW, padH),
      Point2f(dstW - 1 - padW, dstH - 1 - padH),
      Point2f(padW, dstH - 1 - padH)};

  Mat     H = cv::getPerspectiveTransform(ordered_points, destination_points);
  cv::Mat warped;
  cv::warpPerspective(img, warped, H, cv::Size(dstW, dstH));

  return warped;
}
