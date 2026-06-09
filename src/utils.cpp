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

void trunc_left_(string &s) {
  bool f = true;

  while (f) {
    if (s[0] == ' ') {
      s = s.substr(1, s.size() - 1);
    } else {
      f = false;
    }
  }
}

void trunc_right_(string &s) {
  bool f = true;

  while (f) {
    if (s[s.size() - 1] == ' ') {
      s = s.substr(0, s.size() - 1);
    } else {
      f = false;
    }
  }
}

void trunc_(string &s) {
  string res = "  shit   ";
  trunc_left_(res);
  trunc_right_(res);
  CV_Assert(res == "shit");

  trunc_left_(s);
  trunc_right_(s);
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
  int idx = -1;
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

    if (!l.empty()) {
      trunc_(l);
      res[current_file].insert(l);
    }
  }

  return res;
}

template <typename T, typename P> void print_map_firsts(const map<T, P> &mp) {
  for (const auto &i : mp) {
    cout << i.first << endl;
  }
}

void add_found_count(const map<string, set<string>> &found,
                     map<string, map<string, int>> &codes) {

  for (auto it = found.begin(); it != found.end(); ++it) {
    for (auto itt = (it->second).begin(); itt != (it->second).end(); ++itt) {
      // if (codes.find(it->first) == codes->end())
      codes[it->first][*itt] += 1;
    }
  }
}

void compare_found_codes(string found_file, string compare_file_name,
                         string truth_file) {
  map<string, set<string>> found = read_codes(found_file);
  map<string, set<string>> truth = read_codes(truth_file);

  ofstream f(compare_file_name, ios::out);

  int total_tp = 0;
  int total_fp = 0;
  int total_fn = 0;

  for (const auto &[filename, truth_set_raw] : truth) {
    set<string> truth_set = truth_set_raw;
    truth_set.erase("");

    set<string> found_set;
    if (found.count(filename)) {
      found_set = found[filename];
      found_set.erase("");
    }

    vector<string> tp, fp, fn;

    set_intersection(truth_set.begin(), truth_set.end(), found_set.begin(),
                     found_set.end(), back_inserter(tp));

    set_difference(found_set.begin(), found_set.end(), truth_set.begin(),
                   truth_set.end(), back_inserter(fp));

    set_difference(truth_set.begin(), truth_set.end(), found_set.begin(),
                   found_set.end(), back_inserter(fn));

    total_tp += tp.size();
    total_fp += fp.size();
    total_fn += fn.size();

    f << filename << "\n";

    f << "  [TP] (" << tp.size() << "): ";
    for (const auto &s : tp)
      f << s << " ";
    f << "\n";

    f << "  [FP] (" << fp.size() << "): ";
    for (const auto &s : fp)
      f << s << " ";
    f << "\n";

    f << "  [FN] (" << fn.size() << "): ";
    for (const auto &s : fn)
      f << s << " ";
    f << "\n\n";
  }

  for (const auto &[filename, found_set_raw] : found) {
    if (truth.count(filename) == 0) {
      set<string> found_set = found_set_raw;
      found_set.erase("");
      if (!found_set.empty()) {
        total_fp += found_set.size();
        f << filename << " (not in truth)\n";
        f << "  [FP] (" << found_set.size() << "): ";
        for (const auto &s : found_set)
          f << s << " ";
        f << "\n\n";
      }
    }
  }

  double precision = (total_tp + total_fp > 0)
                         ? (double)total_tp / (total_tp + total_fp)
                         : 0.0;
  double recall = (total_tp + total_fn > 0)
                      ? (double)total_tp / (total_tp + total_fn)
                      : 0.0;
  double f1_score = (precision + recall > 0)
                        ? 2.0 * (precision * recall) / (precision + recall)
                        : 0.0;

  f << "Total TP : " << total_tp << "\n";
  f << "Total FP : " << total_fp << "\n";
  f << "Total FN : " << total_fn << "\n";

  f << fixed << setprecision(4);
  f << "Precision: " << precision << "\n";
  f << "Recall   : " << recall << "\n";
  f << "F1-Score : " << f1_score << "\n";

  // // Вывод в формате CSV для копирования в Excel/Google Sheets
  // f << "\n\n=== ДЛЯ ВСТАВКИ В ТАБЛИЦУ (CSV) ===\n";
  // f << "TP,FP,FN,Precision,Recall,F1-Score\n";
  // f << total_tp << "," << total_fp << "," << total_fn << ","
  //   << precision << "," << recall << "," << f1_score << "\n";

  f.close();
}
