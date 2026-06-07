#include "../include/utils.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat localize_bar_code_and_straght(const Mat &binary);
