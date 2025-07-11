#include <unordered_map>
#include <span>
#include <string>
#include <string_view>

namespace senior {
  
std::unordered_map<std::string_view, std::size_t>
first_offsets(std::span<std::string> strings) {
  std::unordered_map<std::string_view, std::size_t> offsets;
  for (std::size_t idx = 0; idx != strings.size(); ++idx)
    offsets.try_emplace(std::string_view(strings[idx]), idx);
  return offsets;
}

}   // namespace senior
