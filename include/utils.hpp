#include <opencv2/core/base.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

void write_found_codes(map<string, set<pair<string, string>>> codes,
                       string filename, bool do_write_code_names = false);

void compare_found_codes(string found_file, string compare_file_name,
                         vector<string> img_names);
