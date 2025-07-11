#include <unordered_map>
#include <string>
#include <vector>

namespace mid {
  
std::unordered_map<std::string, std::size_t>
first_offsets(std::vector<std::string> const & strings) {
  std::unordered_map<std::string, std::size_t> offsets;
  for (std::size_t idx = 0; idx < strings.size(); idx++)
    if (!offsets.contains(strings[idx]))
      offsets[strings[idx]] = idx;
  return offsets;
}

}   // namespace mid