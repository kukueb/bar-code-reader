#include <opencv2/core/base.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

float get_max(vector<float> vec) {
  CV_Assert(vec.size() > 0);
  float mx = vec[0];

  for (auto i : vec) {
    if (mx < i)
      mx = i;
  }
  return mx;
}

float get_min(vector<float> vec) {
  CV_Assert(vec.size() > 0);
  float mx = vec[0];

  for (auto i : vec) {
    if (mx > i)
      mx = i;
  }
  return mx;
}

vector<Point2f> order_points(const vector<Point2f> &points) {
  vector<Point2f> ordered(4);

  vector<float> sums, diffs;
  for (auto p : points) {
    sums.push_back(p.x + p.y);
    diffs.push_back(p.y - p.x);
  }

  float mxsum = get_max(sums);
  float mxdiff = get_max(diffs);
  float mnsum = get_min(sums);
  float mndiff = get_min(diffs);
  for (int i = 0; i < points.size(); ++i) {
    if (sums[i] == mxsum)
      ordered[0] = points[i];
    if (sums[i] == mnsum)
      ordered[2] = points[i];
    if (diffs[i] == mxdiff)
      ordered[1] = points[i];
    if (diffs[i] == mxdiff)
      ordered[3] = points[i];
  }

  return ordered;
}
