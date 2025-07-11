#include <bit>
#include <span>
#include <limits>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <string_view>

namespace enthusiast {

using string_ptr_t = std::string const *;

class flat_unordered_set_t {
    string_ptr_t first {};
    std::vector<string_ptr_t> hashed {};
 public:
    flat_unordered_set_t(std::span<std::string> strings)
        : first(strings.data()), 
          hashed(std::bit_ceil<std::size_t>(strings.size() * 1.3)) {
        std::fill_n(hashed.data(), hashed.size(), nullptr);
    }
    void try_emplace(std::string const & string) {
        auto hash = std::hash<std::string_view> {}(string);
        auto slot = hash & (hashed.size() - 1);
        while (hashed[slot] && *hashed[slot] != string)
            slot = (slot + 1) & (hashed.size() - 1);
        if (!hashed[slot])
            hashed[slot] = &string;
    }
    std::size_t operator[](std::string_view string) const noexcept {
        auto hash = std::hash<std::string_view> {}(string);
        auto slot = hash & (hashed.size() - 1);
        while (hashed[slot] && *hashed[slot] != string)
            slot = (slot + 1) & (hashed.size() - 1);
        return hashed[slot] 
                ? hashed[slot] - first
                : std::numeric_limits<std::size_t>::max();
    }
};

std::optional<flat_unordered_set_t>
first_offsets(std::span<std::string> strings) 
try {
    flat_unordered_set_t offsets {strings};
    for (auto const & string : strings)
        offsets.try_emplace(string);
    return {std::move(offsets)};
}
catch (...) {
    return {};
}

}   // namespace enthusiast
