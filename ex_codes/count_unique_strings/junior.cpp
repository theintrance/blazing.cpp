#include <map>
#include <string>
#include <vector>

namespace junior {

std::map<std::string, int>
first_offsets(std::vector<std::string> strings) {
  std::map<std::string, int> offsets;
  for (int idx = 0; idx < strings.size(); idx++)
    if (offsets.find(strings[idx]) == offsets.end())
      offsets[strings[idx]] = idx;
    return offsets;
}

}   // namespace junior








