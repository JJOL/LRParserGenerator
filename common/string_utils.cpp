#include "string_utils.h"
#include <string>
#include <vector>
#include <regex>
using String = std::string;

std::vector<String> split(String s, String delimiter)
{
    std::vector<String> splits;
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

std::vector<String> split(String s, std::regex r)
{
  std::vector<String> splits;
  std::smatch m;

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