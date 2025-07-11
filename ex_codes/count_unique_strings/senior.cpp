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

/*
senior_first_offsets/size10000_len3_alpha16/0            315989 ns <> 283421 ns       315658 ns         2226 <> 2237
senior_first_offsets/size10000_len3_alpha32/1            539011 ns <> 530790 ns       538492 ns         1288 <> 1317
senior_first_offsets/size10000_len4_alpha16/2            589511 ns <> 532345 ns       581591 ns         1246 <> 1250
senior_first_offsets/size10000_len4_alpha32/3            589669 ns <> 542888 ns       588702 ns         1173 <> 1193
senior_first_offsets/size10000_len5_alpha16/4            583769 ns <> 544882 ns       583227 ns         1180 <> 1195
senior_first_offsets/size10000_len5_alpha32/5            594523 ns <> 572938 ns       592949 ns         1172 <> 1188
senior_first_offsets/size100000_len3_alpha16/6          1457451 ns <> 1287228 ns      1456111 ns          478 <> 490
senior_first_offsets/size100000_len3_alpha32/7          3653790 ns <> 3489215 ns      3643219 ns          192 <> 200
senior_first_offsets/size100000_len4_alpha16/8          5223465 ns <> 4635246 ns      5218806 ns          134 <> 135
senior_first_offsets/size100000_len4_alpha32/9          7512002 ns <> 6901214 ns      7504717 ns           92 <> 97
senior_first_offsets/size100000_len5_alpha16/10         7571607 ns <> 6897684 ns      7562141 ns           92 <> 95
senior_first_offsets/size100000_len5_alpha32/11         7811671 ns <> 7196090 ns      7782451 ns           91 <> 94
senior_first_offsets/size1000000_len3_alpha16/12       12769704 ns <> 11064201 ns     12749833 ns           54 <> 57
senior_first_offsets/size1000000_len3_alpha32/13       17056031 ns <> 14829196 ns     17007381 ns           42 <> 43
senior_first_offsets/size1000000_len4_alpha16/14       19393488 ns <> 16819598 ns     19376000 ns           36 <> 37
senior_first_offsets/size1000000_len4_alpha32/15      128441208 ns <> 128569633 ns    128242200 ns            5 <> 5
senior_first_offsets/size1000000_len5_alpha16/16      139078187 ns <> 140386490 ns    138566750 ns            4 <> 5
senior_first_offsets/size1000000_len5_alpha32/17      218895847 ns <> 206516778 ns    218546333 ns            3 <> 3
====
senior_first_offsets/size10000_len3_alpha16/0        309811 ns       309564 ns         2237
senior_first_offsets/size10000_len3_alpha32/1        534238 ns       533744 ns         1317
senior_first_offsets/size10000_len4_alpha16/2        564752 ns       563924 ns         1250
senior_first_offsets/size10000_len4_alpha32/3        577500 ns       576944 ns         1193
senior_first_offsets/size10000_len5_alpha16/4        577132 ns       576577 ns         1195
senior_first_offsets/size10000_len5_alpha32/5        581640 ns       580841 ns         1188
senior_first_offsets/size100000_len3_alpha16/6      1442984 ns      1440151 ns          490
senior_first_offsets/size100000_len3_alpha32/7      3472917 ns      3470135 ns          200
senior_first_offsets/size100000_len4_alpha16/8      5063177 ns      5058696 ns          135
senior_first_offsets/size100000_len4_alpha32/9      7218447 ns      7202649 ns           97
senior_first_offsets/size100000_len5_alpha16/10     7422132 ns      7350442 ns           95
senior_first_offsets/size100000_len5_alpha32/11     7387075 ns      7381191 ns           94
senior_first_offsets/size1000000_len3_alpha16/12   12355743 ns     12343807 ns           57
senior_first_offsets/size1000000_len3_alpha32/13   16174689 ns     16160930 ns           43
senior_first_offsets/size1000000_len4_alpha16/14   18755797 ns     18732432 ns           37
senior_first_offsets/size1000000_len4_alpha32/15  138564392 ns    138137600 ns            5
senior_first_offsets/size1000000_len5_alpha16/16  134405092 ns    134105800 ns            5
senior_first_offsets/size1000000_len5_alpha32/17  227911264 ns    227073667 ns            3
===
senior_first_offsets/size10000_len3_alpha16/0        283421 ns       283214 ns         2430
senior_first_offsets/size10000_len3_alpha32/1        530790 ns       524878 ns         1355
senior_first_offsets/size10000_len4_alpha16/2        532345 ns       531966 ns         1298
senior_first_offsets/size10000_len4_alpha32/3        542888 ns       542437 ns         1229
senior_first_offsets/size10000_len5_alpha16/4        544882 ns       543657 ns         1285
senior_first_offsets/size10000_len5_alpha32/5        572938 ns       563648 ns         1255
senior_first_offsets/size100000_len3_alpha16/6      1287228 ns      1284886 ns          534
senior_first_offsets/size100000_len3_alpha32/7      3489215 ns      3377405 ns          210
senior_first_offsets/size100000_len4_alpha16/8      4635246 ns      4632901 ns          151
senior_first_offsets/size100000_len4_alpha32/9      6901214 ns      6895653 ns          101
senior_first_offsets/size100000_len5_alpha16/10     6897684 ns      6887480 ns          100
senior_first_offsets/size100000_len5_alpha32/11     7196090 ns      7148790 ns          100
senior_first_offsets/size1000000_len3_alpha16/12   11064201 ns     11027215 ns           65
senior_first_offsets/size1000000_len3_alpha32/13   14829196 ns     14738814 ns           43
senior_first_offsets/size1000000_len4_alpha16/14   16819598 ns     16800150 ns           40
senior_first_offsets/size1000000_len4_alpha32/15  128569633 ns    128353000 ns            5
senior_first_offsets/size1000000_len5_alpha16/16  140386490 ns    139832250 ns            4
senior_first_offsets/size1000000_len5_alpha32/17  206516778 ns    206196667 ns            3
*/