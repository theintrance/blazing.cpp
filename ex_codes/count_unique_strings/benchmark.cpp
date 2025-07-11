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


/*
clang++ -std=c++20 benchmark.cpp \
        -I/opt/homebrew/include \
        -L/opt/homebrew/lib \
        -lbenchmark_main -lbenchmark -lpthread \
        -O3 \
        -o benchmark
=====
junior_first_offsets/size10000_len3_alpha16/0           1184549 ns      1182316 ns          588
mid_first_offsets/size10000_len3_alpha16/0               308165 ns       307614 ns         2154
senior_first_offsets/size10000_len3_alpha16/0            315989 ns       315658 ns         2226
enthusiast_first_offsets/size10000_len3_alpha16/0         71974 ns        71883 ns         9807
junior_first_offsets/size10000_len3_alpha32/1           1943152 ns      1941249 ns          361
mid_first_offsets/size10000_len3_alpha32/1               554051 ns       553570 ns         1255
senior_first_offsets/size10000_len3_alpha32/1            539011 ns       538492 ns         1288
enthusiast_first_offsets/size10000_len3_alpha32/1         61169 ns        61103 ns        10692
junior_first_offsets/size10000_len4_alpha16/2           1818216 ns      1816867 ns          383
mid_first_offsets/size10000_len4_alpha16/2               586607 ns       586053 ns         1194
senior_first_offsets/size10000_len4_alpha16/2            589511 ns       581591 ns         1246
enthusiast_first_offsets/size10000_len4_alpha16/2         50698 ns        50632 ns        12991
junior_first_offsets/size10000_len4_alpha32/3           1987384 ns      1976852 ns          359
mid_first_offsets/size10000_len4_alpha32/3               624183 ns       619069 ns          858
senior_first_offsets/size10000_len4_alpha32/3            589669 ns       588702 ns         1173
enthusiast_first_offsets/size10000_len4_alpha32/3         60719 ns        60669 ns        11002
junior_first_offsets/size10000_len5_alpha16/4           1920286 ns      1918399 ns          366
mid_first_offsets/size10000_len5_alpha16/4               619874 ns       617581 ns         1136
senior_first_offsets/size10000_len5_alpha16/4            583769 ns       583227 ns         1180
enthusiast_first_offsets/size10000_len5_alpha16/4         61662 ns        61586 ns        10755
junior_first_offsets/size10000_len5_alpha32/5           1954135 ns      1952274 ns          358
mid_first_offsets/size10000_len5_alpha32/5               628197 ns       625561 ns         1135
senior_first_offsets/size10000_len5_alpha32/5            594523 ns       592949 ns         1172
enthusiast_first_offsets/size10000_len5_alpha32/5         61128 ns        61066 ns        10898
junior_first_offsets/size100000_len3_alpha16/6          8063313 ns      8053895 ns           86
mid_first_offsets/size100000_len3_alpha16/6             1330442 ns      1329068 ns          468
senior_first_offsets/size100000_len3_alpha16/6          1457451 ns      1456111 ns          478
enthusiast_first_offsets/size100000_len3_alpha16/6       549541 ns       548459 ns         1271
junior_first_offsets/size100000_len3_alpha32/7         15849563 ns     15827227 ns           44
mid_first_offsets/size100000_len3_alpha32/7             3701889 ns      3645732 ns          198
senior_first_offsets/size100000_len3_alpha32/7          3653790 ns      3643219 ns          192
enthusiast_first_offsets/size100000_len3_alpha32/7       885875 ns       884986 ns          776
junior_first_offsets/size100000_len4_alpha16/8         18705775 ns     18688270 ns           37
mid_first_offsets/size100000_len4_alpha16/8             5171325 ns      5166216 ns          134
senior_first_offsets/size100000_len4_alpha16/8          5223465 ns      5218806 ns          134
enthusiast_first_offsets/size100000_len4_alpha16/8      1120592 ns      1119522 ns          613
junior_first_offsets/size100000_len4_alpha32/9         28184478 ns     28104923 ns           26
mid_first_offsets/size100000_len4_alpha32/9             7704581 ns      7698549 ns           91
senior_first_offsets/size100000_len4_alpha32/9          7512002 ns      7504717 ns           92
enthusiast_first_offsets/size100000_len4_alpha32/9      1223602 ns      1222524 ns          565
junior_first_offsets/size100000_len5_alpha16/10        27598663 ns     27571440 ns           25
mid_first_offsets/size100000_len5_alpha16/10            7754514 ns      7746011 ns           87
senior_first_offsets/size100000_len5_alpha16/10         7571607 ns      7562141 ns           92
enthusiast_first_offsets/size100000_len5_alpha16/10     1239106 ns      1237980 ns          549
junior_first_offsets/size100000_len5_alpha32/11        28880752 ns     28813083 ns           24
mid_first_offsets/size100000_len5_alpha32/11            8297175 ns      8244729 ns           85
senior_first_offsets/size100000_len5_alpha32/11         7811671 ns      7782451 ns           91
enthusiast_first_offsets/size100000_len5_alpha32/11     1237643 ns      1236271 ns          560
junior_first_offsets/size1000000_len3_alpha16/12       75137991 ns     75064222 ns            9
mid_first_offsets/size1000000_len3_alpha16/12          11283556 ns     11272328 ns           61
senior_first_offsets/size1000000_len3_alpha16/12       12769704 ns     12749833 ns           54
enthusiast_first_offsets/size1000000_len3_alpha16/12    5794993 ns      5783956 ns           90
junior_first_offsets/size1000000_len3_alpha32/13      114379854 ns    114282167 ns            6
mid_first_offsets/size1000000_len3_alpha32/13          15047660 ns     15030745 ns           47
senior_first_offsets/size1000000_len3_alpha32/13       17056031 ns     17007381 ns           42
enthusiast_first_offsets/size1000000_len3_alpha32/13    6660089 ns      6654667 ns          102
junior_first_offsets/size1000000_len4_alpha16/14      136308475 ns    136187800 ns            5
mid_first_offsets/size1000000_len4_alpha16/14          17641234 ns     17626250 ns           40
senior_first_offsets/size1000000_len4_alpha16/14       19393488 ns     19376000 ns           36
enthusiast_first_offsets/size1000000_len4_alpha16/14    6484389 ns      6477386 ns          101
junior_first_offsets/size1000000_len4_alpha32/15      396384605 ns    396018000 ns            2
mid_first_offsets/size1000000_len4_alpha32/15         117953257 ns    117780333 ns            6
senior_first_offsets/size1000000_len4_alpha32/15      128441208 ns    128242200 ns            5
enthusiast_first_offsets/size1000000_len4_alpha32/15   19687761 ns     19655800 ns           35
junior_first_offsets/size1000000_len5_alpha16/16      400698354 ns    399558500 ns            2
mid_first_offsets/size1000000_len5_alpha16/16         129243937 ns    128850000 ns            6
senior_first_offsets/size1000000_len5_alpha16/16      139078187 ns    138566750 ns            4
enthusiast_first_offsets/size1000000_len5_alpha16/16   23993109 ns     23663613 ns           31
junior_first_offsets/size1000000_len5_alpha32/17      553306584 ns    552259000 ns            1
mid_first_offsets/size1000000_len5_alpha32/17         215753583 ns    215496000 ns            3
senior_first_offsets/size1000000_len5_alpha32/17      218895847 ns    218546333 ns            3
enthusiast_first_offsets/size1000000_len5_alpha32/17   16324328 ns     16279326 ns           43

*/

