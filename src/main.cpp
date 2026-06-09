#include <../include/localization.hpp>
#include <ZXing/ZXingCpp.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

using parse_func_ptr = set<pair<string, string>> (*)(Mat &);

string data_path = "../data/";
string output_path = data_path + "output.txt";

vector<string> img_names = {
    "1.jpg", "2.jpg", "2_no_text.jpg", "3.jpg",  "4.jpg",  "5.jpg",  "6.jpg",
    "7.jpg", "8.jpg", "9.jpg",         "10.jpg", "11.jpg", "12.jpg", "13.jpg",
};

map<string, set<pair<string, string>>> codes;

Mat parse(string img_path, function<set<pair<string, string>>(Mat &)> method) {
  Mat img = imread(data_path + img_path);

  auto new_codes = method(img);
  codes[img_path].merge(new_codes);

  return img;
}

void parse_and_show(string img_path,
                    function<set<pair<string, string>>(Mat &)> method) {
  Mat img = parse(img_path, method);
  imshow("img", img);
}

void find_and_write(function<set<pair<string, string>>(Mat &)> method,
                    string file_name) {
  for (int i = 0; i < img_names.size(); ++i) {
    parse(img_names[i], method);
  }
  write_found_codes(codes,
                    file_name); // DOING WRITING CODES INTO FILE
}

int main() {
  int idx = 0;

  vector<parse_func_ptr> methods = {zbar_code_parse,       inv_rot_code_parse,
                                    clahe_parse,           bilateral_parse,
                                    clahe_bilateral_parse, parse_by_saturation};

  int cur_try = 1;
  for (; cur_try - 1 < methods.size(); ++cur_try) {
    parse_func_ptr current_method = methods[cur_try - 1];

    string current_try = data_path + "try_" + to_string(cur_try) + ".txt";
    string current_comparison =
        data_path + "compare_" + to_string(cur_try) + ".txt";

    find_and_write(current_method, current_try); // DOING FILE WRITING HERE

    // namedWindow("img", WINDOW_NORMAL);

    compare_found_codes(current_try, current_comparison,
                        data_path + "truth2.txt");
  }

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
