#include <../include/localization.hpp>
#include <ZXing/ZXingCpp.h>
#include <fstream>
#include <ios>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

string data_path = "../data/";
string output_path = data_path + "output.txt";

vector<string> img_names = {
    "1.jpg", "2.jpg", "2_no_text.jpg", "3.jpg",  "4.jpg",  "5.jpg",  "6.jpg",
    "7.jpg", "8.jpg", "9.jpg",         "10.jpg", "11.jpg", "12.jpg", "13.jpg",
};

map<string, set<pair<string, string>>> codes;

Mat parse(string img_path) {
  Mat img = imread(data_path + img_path);

  auto new_codes = zbar_code_parse(img);
  codes[img_path].merge(new_codes);

  return img;
}

void parse_and_show(string img_path) {
  Mat img = parse(img_path);
  imshow("img", img);
}

void find_and_write() {
  for (int i = 0; i < img_names.size(); ++i) {
    parse(img_names[i]);
  }
  write_found_codes(codes,
                    data_path + "try_1.txt"); // DOING WRITING CODES INTO FILE
}

int main(int argc, char *argv[]) {
  int idx = 0;

  find_and_write(); // DOING FILE WRITING HERE

  // namedWindow("img", WINDOW_NORMAL);

  compare_found_codes(data_path + "try_1.txt", data_path + "compare_1.txt",
                      img_names);

  /*

  while (true) {

    parse_and_show(img_names[idx]);

    auto key = waitKey(0);
    if (key == ']') {
      (idx + 1 < img_names.size() ? idx++ : idx);
    } else if (key == '[') {
      (idx - 1 >= 0 ? idx-- : idx);
    } else if (key == 'q') {

      break;
    }
  }

  */
  return 0;
}
