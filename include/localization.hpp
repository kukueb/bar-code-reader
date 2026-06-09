#include "../include/utils.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat localize_bar_code_and_straght(const Mat &binary);

set<pair<string, string>> zbar_code_parse(Mat &src);
set<pair<string, string>> inv_rot_code_parse(Mat &src);

set<pair<string, string>> bilateral_parse(Mat &src);
set<pair<string, string>> clahe_parse(Mat &src);
set<pair<string, string>> clahe_bilateral_parse(Mat &src);
