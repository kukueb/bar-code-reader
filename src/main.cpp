#include <../include/localization.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

string data_path = "../data/";

vector<string> img_names = {
    "1.jpg", "2.jpg", "2_no_text.jpg", "3.jpg",  "4.jpg",  "5.jpg",  "6.jpg",
    "7.jpg", "8.jpg", "9.jpg",         "10.jpg", "11.jpg", "12.jpg", "13.jpg",
};

void do_stuff(string img_path) {
  Mat img = imread(data_path + img_path);

  Mat warped = localize_bar_code_and_straght(img);

  imshow("img", img);
  imshow("straight", warped);
}

int main(int argc, char *argv[]) {
  int idx = 0;

  namedWindow("img");
  namedWindow("straight");

  while (true) {

    do_stuff(img_names[idx]);

    auto key = waitKey(0);
    if (key == ']') {
      (idx + 1 < img_names.size() ? idx++ : idx);
    } else if (key == '[') {
      (idx - 1 >= 0 ? idx-- : idx);
    } else if (key == 'q') {
      break;
    }
  }
  return 0;
}
