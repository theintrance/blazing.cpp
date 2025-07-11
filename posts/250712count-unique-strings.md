---
title: 'Counting Strings in C++: 30x Throughput Difference 💬'
description: 'Find unique string faster'
pubDate: 'July 11 2025'
heroImage: 'counting_fingers.png'
categories: ['post']
author: 'Jin-uu'
tags: ['cpp', 'c++', 'string', 'faster', 'benchmark']
---
요즘 채용 단계에서는 단순히 코드를 작성해 문제를 해결하는 것 자체도 중요하지만, 해당 코드를 왜 그렇게 작성했는지 설명할수 있는 능력도 평가에 매우 높은 비중을 차지합니다.

본 포스트에서는 어떤 문제 상황을 코드를 통해 해결하는 방식을 숙련도에 따라 나눠 보고, 해당 방식들에 대해 알아봅니다. 

**주니어** → **숙련자** → **시니어** → **홀릭** 순서대로 코드 구현을 살펴보겠습다. 이후, 각 방식에 대해 실제 성능을 측정해 비교해봅시다. 각 경우의 정의는 다음과 같습니다.

- **주니어**: C++ 튜토리얼을 완료한 초보자
- **숙련자**: C++ 언어 자체에 대한 이해도가 어느정도 존재하는 중급자
- **시니어**: C++에 대한 이해도가 매우 깊은 상급자
- **홀릭**: C++에 대한 깊은 이해도를 기반으로 최대한의 최적화를 고민하는 사람(~~변태~~)

# 문제 상황

> 문자열 (`std::string`)를 담고있는 컨테이너가 존재한다. 이 때, 어떤 유니크한 문자열이 가장 먼저 등장하는 인덱스를 찾아라.
예시) 컨테이너: `{’aaa’, ‘bbb’, ‘aaa’, ‘ccc’}` → 정답: `‘aaa’: 0, ‘bbb’: 1, ‘ccc’: 3`
> 

먼저, 아래 각 방식의 코드를 살펴보기 전에, 본인이 생각하는 ‘효율적인’ 코드를 작성해봅시다. ‘효율적’이라 함은, 시간적, 공간적 효율을 의미합니다.

### Python 풀이

위 문제를 Python으로 해결해보면, 다음과 같이 풀이할 수 있습니다.

```python
def first_offsets(strings: list[str]) -> dict[str, int]:
   offsets = {}
   for idx, string in strings:
      if string not in offsets:
         offsets[string] = idx
   return offsets
```

위 코드를 C++로는 어떻게 구현할 수 있을까요?

# 주니어

C++은 매우 저수준의 추상화부터 높은 수준의 추상화된 클래스를 제공합니다. 주니어 개발자 대부분의 특징은, 어떤 문제 상황을 해결할 때, 어느 정도의 추상화 수준을 선택할 지 고민조차 하지 못한다는 것입니다. 또한, 어떤 컨테이너를 언제 왜 어떻게 사용해야 하는지 확실하게 알지 못합니다.

`std::string`을 담을 컨테이너에 어떤 것을 사용해야 할까요? C++을 배우며 가장 먼저 배우게 되는 컨테이너 중 하나인 `std::vector`를 사용해 봅시다. 또한, 각 유니크한 문자열에 대한 인덱스를 저장할 컨테이너로 `std::map`을 사용해 봅시다.

```cpp
#include <map>
#include <string>
#include <vector>

std::map<std::string, int>
first_offsets(std::vector<std::string> strings) {

   std::map<std::string, int> offsets;
   for (int idx = 0; idx < strings.size(); idx++)
      if (offsets.find(strings[idx]) == offsets.end())
         offsets[strings[idx]] = idx;
   return offsets;
}
```

문자열을 담은 컨테이너 `strings`를 순회하며, `std::map`에 이미 저장되어 있는지 확인 후, 없다면 문자열을 `key`로 해당 인덱스를 `value`로 저장합니다.

주니어 개발자가 작성한 코드에서 수정하고 싶은 부분이 존재하나요? 어떤 부분을 어떻게, 왜 고치면 더 효율적일지 생각해 봅시다.

# 숙련자

C++에 대한 이해도가 어느정도 있는 숙련자는 주니어에 대한 코드를 보고 개선점을 몇가지 찾아낼 수 있을 것입니다.

숙련된 C++ 개발자가 작성한 코드를 살펴봅시다.

```cpp
#include <unordered_map>
#include <string>
#include <vector>

std::unordered_map<std::string, std::size_t>
first_offsets(std::vector<std::string> const & strings) {

   std::unordered_map<std::string, std::size_t> offsets;
   for (std::size_t idx = 0; idx < strings.size(); idx++)
      if (!offsets.contains(strings[idx]))
         offsets[strings[idx]] = idx;
   return offsets;
}
```

개선된 부분들은 다음과 같습니다.

### 불필요한 복사 방지

주니어 코드에서는 `std::vector<std::string>`으로 `strings`를 넘겨주고 있습니다. 함수 호출 시점에 `strings`로 복사 연산이 발생합니다. 따라서 참조 형식을 부여해 불필요한 복사를 방지합니다. 또한, `const`를 부여해 컴파일러에 의한 최적화 여지를 열어둡니다.

### std::unordered_map 사용

`std::map`은 내부적으로 BST를 사용하며, 정렬된 상태를 유지합니다. 본 문제 상황에서는 정렬이 필요하지 않습니다. 따라서, 해시 테이블을 사용하는 컨테이너인 `std::unordered_map`을 사용합니다.

### 불필요한 타입 변환 제거

`idx`는 `int` 타입, `strings.size()`는 `std::size_t` 타입입니다. 따라서 `for`문이 매 번 반복될 때, 타입 변환이 일어나 성능 저하가 발생할 수 있습니다. (물론 컴파일러 최적화 정도에 따라 다를것입니다.) 따라서 `idx`의 타입을 `std::size_t`로 부여합니다.

### 코드 명확성 향상

`offsets.contains()`를 사용해 코드의 Readability를 향상시켰습니다. 단순히 해당 key가 `std::map`에 존재하는지 확인하는 작업이라는 것을 보다 더 간략하고 명확하게 알 수 있습니다.

# 시니어

시니어 개발자는 위 코드에서 더 개선할 만한 점을 찾아낼 수 있습니다.

아래 시니어 개발자가 작성한 코드를 살펴봅시다.

```cpp
#include <unordered_map>
#include <span>
#include <string>
#include <string_view>

std::unordered_map<std::string_view, std::size_t>
first_offsets(std::span<std::string> strings) {
  std::unordered_map<std::string_view, std::size_t> offsets;
  for (std::size_t idx = 0; idx != strings.size(); ++idx)
    offsets.try_emplace(std::string_view(strings[idx]), idx);
  return offsets;
}
```

### offsets 중복 접근 해소

숙련자의 코드를 살펴보면, `key`의 존재 유무를 확인할 때, `key`가 존재하지 않아서 삽입할 때 총 2번 `strings`을 탐색(look-up)합니다. 이를 `try_emplace`를 사용해 한번의 탐색만으로 동일한 동작을 수행하도록 개선했습니다.

### std:span 사용

C++20에서 릴리즈된 `std::span`은 현재의 문제 상황에서 `std::vector`보다 더 좋은 선택이 될 수 있습니다. 예제 코드의 함수에서는 `strings`에 수정 없이 read 연산만 수행되고 있습니다. 또한, 컨테이너 내부 요소의 할당자 역시 신경쓸필요가 없습니다.  따라서 `std::span`을 사용해 동일한 동작을 수행하면서도 `std::span`의 이점을 가지도록 할 수 있습니다. `std::vector` 뿐만 아니라, `std::list`, c-style `array` 등 다양한 컨테이너를 전달받을 수 있습니다.

### std::string_view 사용

예제 코드 함수 구현 상 `offsets`의 `value` 값으로 `std::string` 원본이 들어갈 필요가 없습니다. 따라서 원본 데이터를 가리키는 `std::string_view`를 `value`에 넣어주도록 수정했습니다.

# 홀릭

C++ 언어를 깊게 공부하는 사람들의 상당수는 무언가 ~~변태적인~~ 집요함이 있다. 시니어 개발자의 코드를 조금 더 개선시켜 봅시다.

### unordered_flat_map 구현

`std::unordered_map`은 가장 빠른 해시테이블이 아닙니다. 직접 보다 빠른 map을 구현해 봅시다.

### 인덱스 저장 로직 제거

시니어 개발자의 코드는 `std::unordered_map`에 `std::string`과 `std::size_t`를 저장합니다. `std::unordered_map`은 `key`와 `value`를 `std::pair`로 관리합니다. `std::string_view`은 `16byte`, `std::size_t`는 `8byte`이지만, `std::pair<std::string, std::size_t>`는 `32byte` 입니다. 만약 `std::size_t`를 저장하지 않아도 된다면 `offsets`의 크기는 무려 절반이 줄어들게 됩니다.

아래 코드를 살펴봅시다.

```cpp
#include <bit> // `std::bit_ceil`
#include <span>
#include <limits> // `std::numeric_limits`
#include <string>
#include <vector>
#include <optional>
#include <functional> // `std::hash`
#include <string_view>

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
```

겉보기에는 복잡해 보일 수 있지만, 한 줄씩 살펴보면 간단합니다. `flat_unordered_set_t`는 `std::string_view`의 포인터를 담고있는 `std::vector`를 내부적으로 가지고 있습니다. 즉, 연속된 메모리에 저장된 해시 테이블을 구현한 코드입니다.

또한, 원본 리스트의 시작 주소를 내부적으로 저장하며, 이를 통해 index 값을 따로 저장하지 않고도 특정 문자열의 위치를 알 수 있습니다. `실제 문자열 데이터 주소값 - 원본 리스트의 시작 주소값`을 통해 `index`를 구할 수 있습니다.

### 부가 사항

- C++ 표준에 `std::flat_unordered_set`이 정식 배포되었으면 좋았겠지만, C++23에는 `std::flat_set`만 포함되었습니다. 정렬된 배열은 대부분의 경우 `std::memmove`가 너무 많이 발생해 성능이 떨어집니다. 그래서 최소한의 `flat_unordered_set_t`를 직접 구현했습니다.
- 16 GB RAM으로 10 TB 데이터를 처리하다가 프로그램이 터지는 걸 방지하고자 ‘Function-try-block’을 사용했습니다. 동적 할당은 `std::bad_alloc`을 일으킬 수 있으므로, 예외를 `catch`해 요청을 처리할 수 없다는 메시지를 주는 편이 프로그램이 터지는 것보다는 더 낫겠죠?
- C++23에서는 `std::expected`가 도입되어 앞서 언급한 방식을 보완할 수 있지만, 아직 STL 구현체에 없으므로 `std::optional`을 썼습니다.
- C++20 `<bit>` 헤더의 `std::bit_ceil`로 다음 2의 거듭제곱을 쉽게 구했습니다.

# 벤치마킹 결과

이제 네 경우의 코드에 대해 벤치마크를 수행해 봅시다.

벤치마크는 Mac M4 환경에서 `-O3` 옵션을 부여해 `Clang++`로 컴파일해 Google Benchmark를 사용해 진행되었습니다.

| **데이터 크기** | **알파벳 크기** | **문자열 길이** | **주니어 (ns)** | **숙련자 (ns)** | **시니어 (ns)** | **홀릭 (ns)** |
| --- | --- | --- | --- | --- | --- | --- |
| **1M** | **32** | **3** | 114,282,167 | 15,030,745 | 14,748,399 | 6,654,667 |
| **1M** | **32** | **4** | 396,018,000 | 117,780,333 | 144,928,458 | 19,655,800 |
| **1M** | **32** | **5** | 552,259,000 | 215,496,000 | 233,633,125 | 16,279,326 |
| **1M** | **16** | **3** | 75,064,222 | 11,272,328 | 10,916,212 | 5,783,956 |
| **1M** | **16** | **4** | 136,187,800 | 17,626,250 | 17,305,833 | 6,477,386 |
| **1M** | **16** | **5** | 399,558,500 | 128,850,000 | 145,202,683 | 23,663,613 |

| **데이터 크기** | **알파벳 크기** | **문자열 길이** | **주니어 (ns)** | **숙련자 (ns)** | **시니어 (ns)** | **홀릭 (ns)** |
| --- | --- | --- | --- | --- | --- | --- |
| **1M** | **32** | **3** | 114,282,167 | 15,030,745 | 14,748,399 | 6,654,667 |
| **100K** | **32** | **4** | 28,104,923 | 7,698,549 | 7,360,521 | 1,222,524 |
| **10K** | **32** | **5** | 1,952,274 | 625,561 | 554,311 | 61,066 |

홀릭 개발자가 작성한 코드가 주니어 개발자가 작성한 코드보다 약 30배 정도 빠른 성능을 보였습니다..! 이 측정은 단일 스레드 환경에서 진행되었는데, 만약 다중 스레드 환경이었다면, 메모리 할당 등 때문에 격차는 더 벌어질 것입니다.

# 마무리

이처럼 개발자가 어떤 언어에 대해 어느정도의 이해도, 전문성을 가지고 구현하냐에 따라 간단한 문제도 결과가 크게 갈립니다. 따라서 언어, 특히 C++을 학습하거나 사용할때에는 해당 코드가 왜 사용되었고, 장단점이 무엇인지, 왜 그런지 등 원론적인 내용을 파악해 적용하는 것이 중요합니다. 그리고 필요한 경우 직접 컨테이너등을 구현하는 능력을 기르는 것도 중요합니다.

