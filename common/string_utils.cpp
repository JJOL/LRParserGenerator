#include "string_utils.h"
#include <string>
#include <vector>
#include <regex>

using namespace std;

vector<string> split(string s, string delimiter)
{
    vector<string> splits;
    int delLen = delimiter.length();
    int split_on = -1;

    while((split_on = s.find(delimiter)) != -1) {
        splits.push_back(s.substr(0, split_on));
        s = s.substr(split_on+delLen);
    }

    if(!s.empty()) {
        splits.push_back(s);
    }

    return splits;
}

vector<string> split(string s, regex r)
{
  vector<string> splits;
  smatch m;

  while (regex_search(s, m, r))
  {
    int split_on = m.position();
    splits.push_back(s.substr(0, split_on));
    s = s.substr(split_on + m.length());
  }

  if(!s.empty()) {
    splits.push_back(s);
  }

  return splits;
}