#include <fstream>
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

template <typename T> void print_collection(const T &col) {
  for (const auto &i : col) {
    cout << i << " ";
  }
  cout << endl;
}

void write_found_codes(map<string, set<pair<string, string>>> codes,
                       string filename, bool do_write_code_names = false) {
  ofstream f;
  f.open(filename, ios::out);

  for (auto it = codes.begin(); it != codes.end(); ++it) {
    auto img = it->first;
    auto le_codes = it->second;

    f << img << ":" << endl;
    for (auto itt = le_codes.begin(); itt != le_codes.end(); ++itt) {
      if (do_write_code_names) {
        f << "[" << itt->first << "]: " << itt->second << endl;
      } else {
        f << itt->second << endl;
      }
    }
    f << endl;
  }
  f.close();
}

map<string, set<string>> read_codes(string filename) {
  ifstream file;
  file.open(filename);
  map<string, set<string>> res;

  string l;
  int    idx = -1;
  string current_file;

  while (!file.eof()) {
    idx++;
    getline(file, l);
    int semicolon_idx = l.find(':');

    if (semicolon_idx != -1) {
      if (res[current_file].empty() && !current_file.empty())
        res[current_file].insert("");
      current_file = l.substr(0, semicolon_idx);
      continue;
    }

    if (!l.empty())
      res[current_file].insert(l);
  }

  return res;
}

template <typename T, typename P> void print_map_firsts(const map<T, P> &mp) {
  for (const auto &i : mp) {
    cout << i.first << endl;
  }
}

void add_found_count(const map<string, set<string>> &found,
                     map<string, map<string, int>>  &codes) {

  for (auto it = found.begin(); it != found.end(); ++it) {
    for (auto itt = (it->second).begin(); itt != (it->second).end(); ++itt) {
      // if (codes.find(it->first) == codes->end())
      codes[it->first][*itt] += 1;
    }
  }
}

void compare_found_codes(string found_file, string compare_file_name,
                         vector<string> img_names) {

  map<string, set<string>> found, truth;
  found = read_codes(found_file);
  truth = read_codes("../data/truth.txt");

  ofstream f;
  f.open(compare_file_name, ios::out);

  auto it_truth = truth.begin();
  auto it_found = found.begin();

  while (it_truth != truth.end()) {
    f << it_truth->first << ":" << endl;

    set<string> truth_set = it_truth->second;
    set<string> found_set = it_found->second;

    auto it_set_truth = truth_set.begin();
    auto it_set_found = found_set.begin();

    bool is_something_left = true;

    while (is_something_left) {
      if (it_set_truth != truth_set.end())
        f << "[TRUTH]: " << *(it_set_truth++) << endl;
      if (it_set_found != found_set.end())
        f << "[FOUND]: " << *(it_set_found++) << endl;

      f << endl;

      if (it_set_truth == truth_set.end() && it_set_found == found_set.end())
        is_something_left = false;
    }

    f << endl;

    // going forward
    it_found++;
    it_truth++;
  }
}
