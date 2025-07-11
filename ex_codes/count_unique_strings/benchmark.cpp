#include <benchmark/benchmark.h>
#include <string>
#include <cstddef>
#include <vector>
#include <map>
#include <unordered_map>
#include <optional>
#include <span>

#include "./junior.cpp"
#include "./mid.cpp"
#include "./senior.cpp"
#include "./enthusiast.cpp"

#define STRING_LIST_SIZE_10K 10'000
#define STRING_LIST_SIZE_100K 100'000
#define STRING_LIST_SIZE_1M 1'000'000

#define ALPHABET_SIZE_16 16
#define ALPHABET_SIZE_32 32

#define STRING_LENGTH_3 3
#define STRING_LENGTH_4 4
#define STRING_LENGTH_5 5

namespace bm = benchmark;

static const std::vector<std::size_t> string_list_sizes = {
  STRING_LIST_SIZE_10K,
  STRING_LIST_SIZE_100K,
  STRING_LIST_SIZE_1M,
};

static const std::vector<std::size_t> string_lengths = {
  STRING_LENGTH_3,
  STRING_LENGTH_4,
  STRING_LENGTH_5,
};

static const std::vector<std::size_t> alphabet_sizes = {
  ALPHABET_SIZE_16,
  ALPHABET_SIZE_32,
};

static std::vector<std::string> string_list_generator(const std::size_t string_count, const std::size_t alphabet_size, const std::size_t string_length) {
  static constexpr char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::vector<std::string> strings;
  strings.reserve(string_count);
  for (std::size_t i = 0; i < string_count; ++i) {
    std::string str;
    str.reserve(string_length);
    for (std::size_t j = 0; j < string_length; ++j) {
        str += charset[rand() % alphabet_size];
    }
    strings.push_back(str);
  }
  return strings;
}

// 각 네임스페이스의 first_offsets 선언
namespace junior {
std::map<std::string, int> first_offsets(std::vector<std::string> strings);
}
namespace mid {
std::unordered_map<std::string, std::size_t> first_offsets(std::vector<std::string> const & strings);
}
namespace senior {
std::unordered_map<std::string_view, std::size_t> first_offsets(std::span<std::string> strings);
}
namespace enthusiast {
std::optional<class flat_unordered_set_t> first_offsets(std::span<std::string> strings);
}


// 벤치마크용 문자열 리스트를 미리 생성해두기 위한 구조체
struct StringListFixture {
  std::vector<std::vector<std::string>> string_lists;
  std::vector<std::string> case_names;

  StringListFixture() {
    for (auto size : string_list_sizes) {
      for (auto len : string_lengths) {
        for (auto alpha : alphabet_sizes) {
          string_lists.push_back(string_list_generator(size, alpha, len));
          // case_names에 각 케이스의 이름을 추가
          case_names.push_back(
            "size" + std::to_string(size) +
            "_len" + std::to_string(len) +
            "_alpha" + std::to_string(alpha)
          );
        }
      }
    }
  }
};

static StringListFixture global_fixture;

// 벤치마크 함수 정의
static void BM_junior_first_offsets(benchmark::State& state) {
  // state.range(0): string_lists의 인덱스
  const auto& strings = global_fixture.string_lists[state.range(0)];
  for (auto _ : state) {
    auto result = junior::first_offsets(strings);
    benchmark::DoNotOptimize(result);
  }
}
static void BM_mid_first_offsets(benchmark::State& state) {
  const auto& strings = global_fixture.string_lists[state.range(0)];
  for (auto _ : state) {
    auto result = mid::first_offsets(strings);
    benchmark::DoNotOptimize(result);
  }
}
static void BM_senior_first_offsets(benchmark::State& state) {
  const auto& strings = global_fixture.string_lists[state.range(0)];
  for (auto _ : state) {
    auto result = senior::first_offsets(std::span<std::string>(const_cast<std::vector<std::string>&>(strings)));
    benchmark::DoNotOptimize(result);
  }
}
static void BM_enthusiast_first_offsets(benchmark::State& state) {
  const auto& strings = global_fixture.string_lists[state.range(0)];
  for (auto _ : state) {
    auto result = enthusiast::first_offsets(std::span<std::string>(const_cast<std::vector<std::string>&>(strings)));
    benchmark::DoNotOptimize(result);
  }
}

// 벤치마크 등록
// string_lists의 모든 경우에 대해 벤치마크를 등록
static void RegisterAllBenchmarks() {
  for (std::size_t idx = 0; idx < global_fixture.string_lists.size(); ++idx) {
    benchmark::RegisterBenchmark(
      ("junior_first_offsets/" + global_fixture.case_names[idx]).c_str(),
      &BM_junior_first_offsets
    )->Arg(idx);
    benchmark::RegisterBenchmark(
      ("mid_first_offsets/" + global_fixture.case_names[idx]).c_str(),
      &BM_mid_first_offsets
    )->Arg(idx);
    benchmark::RegisterBenchmark(
      ("senior_first_offsets/" + global_fixture.case_names[idx]).c_str(),
      &BM_senior_first_offsets
    )->Arg(idx);
    benchmark::RegisterBenchmark(
      ("enthusiast_first_offsets/" + global_fixture.case_names[idx]).c_str(),
      &BM_enthusiast_first_offsets
    )->Arg(idx);
  }
}

// main 함수에서 벤치마크 실행
int main(int argc, char** argv) {  
  RegisterAllBenchmarks();
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  return 0;
}
