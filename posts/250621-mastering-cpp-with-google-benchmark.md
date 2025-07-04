---
title: 'Mastering C++ with Google Benchmark ⏱️'
description: 'Google Benchmark & C++'
pubDate: 'July 3 2025'
heroImage: 'google_cpp_fusion.png'
categories: ['post']
author: 'Jin-uu'
tags: ['cpp', 'c++', 'google-benchmark']
---
본 포스트는 [Google Benchmark](https://github.com/google/benchmark)를 통해 **C++로 작성된 코드의 벤치마킹**을 어떻게 수행하는지, 그리고 이 과정에서 **주의해야할 점**, **유용한 기능**에 대해 설명합니다.

# Google Benchmark 기초 사용법

## 함수 정의 & Benchmark 시작

**Google Benchmark**(이하 **GB**)의 사용법은 매우 간단합니다. C++ **함수를 정의**한 뒤, GB에서 제공하는 C-style **macro에 넘겨**주면 끝입니다!

```cpp
#include <benchmark/benchmark.h>

void target_func(benchmark::State& state) {...}

BENCHMARK(target_func);

```

벤치마크를 수행하려면 `BENCHMARK(target_func);` 처럼 성능을 측정하고 싶은 **함수 포인터를 전달**해야 합니다.

해당 함수는 항상 **위 예시와 같은 형태로 정의**되어야 합니다. (`return` 타입은 항상 `void`, parameter type은 항상`benchmark::State&`)

**`benchmark::State`**는 내부적으로 **iterator를 가지고 있으며**, 이를 통해 **range-based for**에서 사용할 수 있습니다. 벤치마크를 수행하며 이 iterator를 통해 객체 내부에서 **“충분히 정확한 측정이 수행됐다”**고 판단될 때까지 **반복적으로 코드를 실행**합니다. 아래 예시를 살펴봅시다.

```cpp
#include <benchmark/benchmark.h>
namespace bm = benchmark;

static void i32_addition(bm::State &state) {
    int32_t a, b, c;
    for (auto _ : state)
        c = a + b;
}

BENCHMARK(i32_addition);
```

`BENCHMARK`가 실행되면, `i32_addition` 내부 for-loop를 돌며 **`c = a + b;`의 코드를 반복적으로 실행**하게 됩니다. 각 반복마다 라이브러리는 `state.KeepRunning()`을 호출하며 내부 기준(최소 시간, 최소 반복 횟수, …)을 만족할 때까지 **반복 횟수를 동적으로 조정**합니다.

최종 결과는 **ns/op** 형태로 출력되며, 이는 **총 소요 시간 / 반복 횟수**를 의미합니다. 따라서 결과값은 `c = a + b;`이 **‘단 한 번’ 수행되는 시간을 의미**합니다.

아래는 위 예시의 실행 결과입니다. *컴파일러는 Clang을 사용, `-O3` 최적화 옵션을 부여했습니다.*

```bash
-------------------------------------------------------
Benchmark             Time             CPU   Iterations
-------------------------------------------------------
i32_addition      0.000 ns        0.000 ns   1000000000000
```

결과를 보면 **함수명**과 **총 시간**, **CPU 사용시간**, **수행한 반복 수**(for-loop를 반복한 횟수)를 알 수 있습니다.

그런데 이상한 점이 있습니다. 바로 **수행시간이 `0.0ns`**라는 것입니다. 아무리 간단한 연산(`c = a + b;`)이라도 `0.0ns`가 정상적인 결과일까요? 위 벤치마크는 MacOS(M4) 환경에서 수행되었으며, M4는 4.05GHz의 클럭속도를 갖고 있습니다. 단순한 덧셈 연산이라 1클럭을 소모했다고 가정하면, 약 `0.25ns`가 소모되었어야 합니다.

눈치가 빠른 분들은 이미 알아차렸겠지만, 위 코드를 컴파일 할때, `-O3` 최적화 옵션이 부여되면서, **`c = a + b;` 연산을 제외하고 컴파일**했기 때문입니다. c가 이후 어디에도 쓰이지 않으니 컴파일러 입장에서는 덧셈 연산이 필요 없는 것이겠죠.

따라서, 정상적으로 `i32_addition` 함수를 측정하려면, **컴파일러가 `c = a + b;` 를 제거하지 않도록 방지**해야 합니다.

## 최적화 방지

**최적화를 방지하는 방법**은 여러가지가 존재합니다.

1. GB에서 제공하는 최적화 방지 함수 사용
2. 최적화 옵션을 피할 수 있도록 트릭 사용
3. …

보통은 위 방법들을 **적절하게 조합**하여 사용합니다. 최적화 방지를 적용한 아래 예시 코드를 살펴봅시다.

```cpp
static void i32_addition_no_op(bm::State &state) {
    int32_t a, b, c;
    for (auto _ : state)
        bm::DoNotOptimize(c = a + b);
}

static void i32_addition_semi_random(bm::State &state) {
    int32_t a = std::rand(), b = std::rand(), c = 0;
    for (auto _ : state)
        bm::DoNotOptimize(c = (++a) + (++b));
}
```

```bash
-------------------------------------------------------------------
Benchmark                         Time             CPU   Iterations
-------------------------------------------------------------------
i32_addition_no_op            0.228 ns        0.228 ns   3101530827
i32_addition_semi_random      0.226 ns        0.226 ns   3095318110
```

`i32_addition_no_op` 함수에서는 **`bm::DoNotOptimize` 를 사용해 최적화를 방지**했습니다.  벤치마크 결과는 **0.228ns**로, 1사이클 만에 연산(`ADD`)이 완료되었음을 알 수 있습니다. 드디어 정상적인 결과를 얻었습니다!

다음으로 `i32_addition_semi_random` 를 살펴봅시다. 이 함수는 **`bm::DoNotOptimize`** 와 **랜덤값 초기화**, **mutation**을 적절히 조합해 최적화를 방지합니다. `a`, `b`를 `std::rand()`로 초기화해 컴파일 타임에 값을 예측할 수 없도록 하였고, 매 반복마다 `INC` 연산을 수행한 뒤, `ADD` 연산을 수행합니다. 결과값은 `i32_addition_no_op` 와 **동일하게 1사이클만에 연산이 수행**되었습니다.

그런데 결과값에 이상한 부분이 존재합니다. `c = (++a) + (++b)` 가 어떻게 1 사이클만에 완료되었을까요? 두 번의 `INC` 연산이 같은 사이클에 서로 다른 ALU 포트에서 실행된다고 해도, 그 결과를 사용하는 `ADD` 연산은 최소 한 사이클 뒤에야 실행할 수 있으므로 **2 사이클이 필요**합니다. 그럼에도 1 사이클만에 연산이 완료된 것처럼 보이는 이유는, 파이프라인이 충분히 채워진 뒤에는 **CPU 코어 내부에서 각 루프의 연산들이 겹쳐 실행**되기 때문입니다. 표로 정리하면 아래와 같습니다.

| 사이클 | 1 | 2 | 3 | 4 | … |
| --- | --- | --- | --- | --- | --- |
| ALU-1 | INC(a1) | ADD(a1+b1) | ADD(a2+b2) | ADD(a3+b3) | … |
| ALU-2 | INC(b1) | INC(a2) | INC(a3) | INC(a4) | … |
| ALU-3 |  | INC(b2) | INC(b3) | INC(b4) | … |

`i32_addition_semi_random` 의 각 loop 연산은 **실제로 2 사이클을 거쳐 완료**되지만, 파이프라인 중첩 덕분에 결과값은 **1 사이클만에 수행된 것처럼 나타나는 것**입니다.

즉, **GB가 제시하는 결과값은 단순한 지연 시간(latency)이 아니라 처리량(throughput)**을 기반으로한 결과인 것입니다.

그러나 연산이 복잡해질수록 `i32_addition_semi_random` 에서 나타나는 이러한 착시 효과는 줄어들 것입니다. 예를 들어 `100,000,000` 사이클이 필요한 연산에서 한두 사이클 차이는 결과값에 거의 영향을 미치지 않습니다.

## 스레드를 사용한 Benchmark

### Micro-benchmark에서의 스레딩

GB는 스레딩을 사용한 벤치마크를 지원합니다. 아래 예시 코드를 살펴봅시다.

```cpp
static void i32_addition_no_op(bm::State &state) {
    int32_t a, b, c;
    for (auto _ : state)
        bm::DoNotOptimize(c = a + b);
}

BENCHMARK(i32_addition_no_op);
BENCHMARK(i32_addition_no_op)->Threads(8);
```

위처럼 `BENCHMARK` macro에 `Threads(n)`을 같이 넘겨주면 됩니다. 아주 간단하지요? 아래 결과를 봅시다.

```bash
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
i32_addition_no_op                      0.229 ns        0.228 ns   3090234858
i32_addition_no_op/threads:8            0.351 ns        0.348 ns   2041485912
```

결과 출력에 자동으로 `thread:8`이라고 친절하게 알려줍니다. 그런데, 이번에도 결과값이 조금 의아합니다. Thread만 늘렸을 뿐인데 결과값이 `0.229ns` → `0.351ns`로 약 1.5배 증가했습니다. 이유가 무엇일까요?

스레딩을 통해 벤치마크를 수행하게 되면, **GB 라이브러리 내부에서 알아서 스레드를 동기화하고 관리**합니다. 이런 작업의 **오버헤드가 결과에 영향**을 끼친 것입니다. 결국 이 역시 마이크로-벤치마킹 작업의 특징으로, 크기가 매우 작은 연산을 측정하다 보니 **외부 요인의 영향이 비교적 크게 작용**하는 것입니다.

이처럼, 아주 간단한 연산에 대한 마이크로-벤치마킹를 수행할 때는 **실행 환경**, **각 loop의 연산 연속성**, **최적화 여부** 등 다양한 요인을 고려해 결과값을 해석해야 하며, 노이즈가 발생하지 않도록 주의해야 합니다.

### 스레딩 사용시 주의점

스레딩을 사용한 벤치마킹에는 주의해야 할 점이 또 존재합니다.

```cpp
static void i32_addition_random(bm::State &state) {
    int32_t c = 0;
    for (auto _ : state)
        c = std::rand() + std::rand();
}
BENCHMARK(i32_addition_random);
BENCHMARK(i32_addition_random)->Threads(8);
```

`i32_addition_random` 함수는 `a`, `b`를 랜덤 값으로 초기화 하고, `ADD` 연산을 수행합니다. `c = std::rand() + std::rand();` 에서 컴파일 타임에 `rand()`값의 결과를 알지 못하므로 컴파일러는 해당 라인을 제거하지 않습니다. 이 함수를 **8개의 스레드를 부여해서 벤치마크를 수행**해 봅시다.

```bash
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
i32_addition_random                      7.41 ns         7.40 ns     95251055
i32_addition_random/threads:8            10.5 ns         10.4 ns     67319024
```

결과를 보면, 스레드로 수행한 결과가 약 `3ns`, 1.4배 느려졌습니다. 왜 이런 결과가 나왔을까요? `std::rand()` 는 내부에서 mutex-lock 연산을 수행합니다. 따라서 **스레드가 늘어날수록 스레드끼리 서로 간섭**을 하게 되고 이에 대한 **오버헤드가 증가**합니다.

이처럼, thread를 부여해 벤치마크를 수행할때는, mutex-lock연산과 같이 **global하게 적용되는 연산 사용을 주의**해야 합니다.

# 수학 수식과 최적화

이제 C++에서 자주 사용되는 수학 수식과 이를 어떻게 최적화 할 수 있을지 알아봅시다.

### 곱셈과 덧셈 조합 최적화

아래 간단한 `std::sin()` 예시를 봅시다.

```cpp
static void f64_sin(bm::State &state) {
    double argument = std::rand(), result = 0;
    for (auto _ : state)
        bm::DoNotOptimize(result = std::sin(argument += 1.0));
}
BENCHMARK(f64_sin);
```

아래는 결과 출력입니다.

```bash
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
f64_sin                                3.44 ns         3.43 ns    192021770
```

`std::sin` 은 **내부에서 입력값의 범위, 특수값(INF, NaN)등 여러 예외에 대한 처리로직이 포함**되어 있습니다. 이를 제거하고, 순수한 sin 연산만 수행해 봅시다.

```cpp
static void f64_sin_pure(bm::State &state) {
    double argument = std::rand(), result = 0;
    for (auto _ : state) {
        argument += 1.0;
        result = (argument) - (argument * argument * argument) / 6.0 +
                 (argument * argument * argument * argument * argument) / 120.0;
        bm::DoNotOptimize(result);
    }
}
BENCHMARK(f64_sin_pure);
```

이전 함수와 **동일한 연산 결과를 제공**하지만, **예외 처리는 제거**한 함수입니다. 컴파일러는 거듭 제곱등 여러 연산을 최적화할 수 있습니다. 아래 결과를 살펴봅시다.

```cpp
---------------------------------------------------------------------------
Benchmark                                 Time             CPU   Iterations
---------------------------------------------------------------------------
f64_sin_pure                           1.20 ns         1.20 ns    586903664
```

무려 **약 3배 빨라**졌습니다! 단순하게 컴파일러에게 최적화 여지를 제공하기만 해도 큰 성능 향상을 이끌어낼 수 있습니다. 여기서 더 빠르게 만들 수 있을까요? `—fast-math` 컴파일 옵션을 사용하면 가능합니다!

```cpp
[[gnu::optimize("-ffast-math")]]
static void f64_sin_fast_math(bm::State &state) {
    double argument = std::rand(), result = 0;
    for (auto _ : state) {
        argument += 1.0;
        result = (argument) - (argument * argument * argument) / 6.0 +
                 (argument * argument * argument * argument * argument) / 120.0;
        bm::DoNotOptimize(result);
    }
}
BENCHMARK(f64_sin_fast_math);
```

```bash
------------------------------------------------------------
Benchmark                  Time             CPU   Iterations
------------------------------------------------------------
f64_sin_fast_math      0.655 ns        0.654 ns   1071434038
```

무려 `f64_sin`과 비교했을때는 약 **5배**, `f64_sin_pure`와는 약 **1.8배** 향상됐습니다! **`-fast-math`가 연산 순서를 최적화**해 이와 같은 결과를 얻을 수 있었던 것입니다.

이처럼 **컴파일러가 내부까지 최적화 하지 못하는 연산을 풀어서 쓰기만 해도 성능이 비약적으로 향상될 수 있습니다.**

### 나눗셈 최적화

CPU에서 가장 느린 연산 중 하나는 나누기(`DIV` / `IDIV`) 연산입니다. 평균적으로 덧셈(`ADD`)은 1사이클, 곱셈(`IMUL`)은 3사이클인데 반해, 나눗셈(`IDIV`)은 십수 사이클이 필요합니다. 이 때문에, 만약 **나눗셈이 병목인 프로그램이 존재한다면, 이제 설명할 최적화 기법은 큰 도움**이 될 수 있을 것입니다.

```cpp
static void i64_division_by_constexpr(bm::State &state) {
    constexpr int64_t b = 2147483647;
    int64_t a = std::rand(), c;
    for (auto _ : state)
        bm::DoNotOptimize(c = (++a) / b);
}
BENCHMARK(i64_division_by_constexpr);
```

**컴파일러에게 나눗셈 연산의 분모가 상수(`const`)값이라는 것을 알려주기만 해도 큰 최적화 효과**를 이끌어 낼 수 있습니다. `c = (++a) / b` 연산은 십수 사이클이 필요하므로 약 **3ns** 가 소요될 것입니다. 그러나, 분모 b가 상수임을 컴파일러에게 알려주면 결과는 어떨까요?

```bash
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
i64_division_by_constexpr      0.314 ns        0.314 ns   2227674721
```

무려 **10배** 가까이 성능 향상이 이루어졌습니다.

이처럼 `DIV` 연산을 수행할 때, **컴파일 타임에 분모의 값을 알 수 있는 경우, 컴파일러에게 알려주는 것이 매우 중요**합니다.

# 메모리 접근

C++ 개발자들과 애증의 관계인 **메모리 관리 / 메모리 접근은 최적화 측면에서도 중요**합니다. 특히 **캐시 미스를 줄이는 것**이 자잘한 최적화보다 훨씬 더 큰 성능 향상을 이끌어낼 수 있습니다.

아래 예시를 살펴봅시다

```cpp
// Mac M4: 128 bytes
constexpr std::size_t cache_line_bytes           = 128;
constexpr std::size_t f32s_in_cache_line_k       = cache_line_bytes / sizeof(float);   // 32
constexpr std::size_t f32s_in_cache_line_half_k  = f32s_in_cache_line_k / 2;           // 16

struct alignas(cache_line_bytes) f32_array_t {
    float raw[f32s_in_cache_line_k * 2];
};
```

> *위 예제 코드는 MacOS M4를 기준으로 작성되었습니다. M4의 캐시 라인 사이즈는 128byte이며 다른 환경일 경우 적절하게 조절해야 합니다.*
> 

위와 같이 `f32_array_t` 구조체를 정의합니다. `alignas(cache_line_bytes)`를 사용해 `f32_array_t`는 메모리 상 128byte의 배수 단위에 위치합니다. 따라서 `raw`는 한 개의 캐시라인으로는 배열 전체에 접근하지 못하며, **정확히 2개의 캐시 라인이 로드되어야 배열 전체에 접근할 수 있습니다.**

위 내용을 이해한 뒤, 다음 예제를 살펴봅시다.

```cpp
static void f32_pairwise_accumulation(bm::State &state) {
    f32_array_t a, b, c;
    for (auto _ : state)
        for (size_t i = f32s_in_cache_line_half_k; i != f32s_in_cache_line_half_k * 3; ++i)       // 16 ~ 47
            bm::DoNotOptimize(c.raw[i] = a.raw[i] + b.raw[i]);
}

static void f32_pairwise_accumulation_aligned(bm::State &state) {
    f32_array_t a, b, c;
    for (auto _ : state)
        for (size_t i = 0; i != f32s_in_cache_line_k; ++i) // 0 ~ 31
            bm::DoNotOptimize(c.raw[i] = a.raw[i] + b.raw[i]);
}
```

**두 함수는 모두 같은 동작을 수행**하지만, 루프 범위가 다릅니다. `f32_pairwise_accumulation`은 16 ~ 47을 순회하며, `f32_pairwise_accumulation_aligned` 0 ~ 31를 순회합니다. 두 함수 모두 같은 횟수의 루프를 돌며 같은 동작을 수행합니다. 직관적으로는 두 함수의 결과치는 똑같을 것 같지만 **실제 결과는 그렇지 않습니다.**

```bash
----------------------------------------------------------------------------
Benchmark                                  Time             CPU   Iterations
----------------------------------------------------------------------------
f32_pairwise_accumulation              0.733 ns        0.732 ns    957304231
f32_pairwise_accumulation_aligned      0.522 ns        0.521 ns   1340251584
```

결과는 0 ~ 31 루프를 도는 `f32_pairwise_accumulation_aligned`가 약 **1.4배** 정도 빠른 성능을 보였습니다. 이 이유는 **캐시로드** 때문인데요, `raw` 배열은 258byte, `float` 64개로 이루어져 있습니다. 따라서 하나의 캐시라인으로는 `raw`를 모두 불러올 수 없습니다. 그러나 0~31 루프만을 돈다면, 하나의 캐시라인으로도 해결할 수 있습니다. 따라서 `f32_pairwise_accumulation_aligned`는 `f32_pairwise_accumulation`와는 다르게 **cache miss가 한 번만 일어납니다.**

이처럼 로직과는 별개로 **메모리 로드 때문에 벤치마크 결과에 노이즈가 발생**할 수 있습니다. 이에 주의해서 측정 코드를 구성해야합니다.

# **~~Cost of Branching~~**

# GB 고급 사용

GB에서 제공하는 부가적인 기능 중 꽤 유용한 기능들이 존재합니다. 이 중 몇가지를 소개합니다.

### 시간 측정 일시 정지

- `state.PauseTiming()`
- `state.ResumeTiming()`

위 두 함수는 **벤치마킹 도중 시간 측정을 일시 정지**하고 싶을 때 사용합니다. 아래 실제 예시를 살펴봅시다.

```cpp
static void upper_cost_of_pausing(bm::State &state) {
    int32_t a = std::rand(), c = 0;
    for (auto _ : state) {
        state.PauseTiming();
        ++a;    // 측정 제외
        state.ResumeTiming();
        bm::DoNotOptimize(c += a);
    }
}
```

`state.PauseTiming()` 와 `state.ResumeTiming()` 를 사용해 `++a` 연산을 수행할 때 시간 측정을 멈춥니다. 그럼 결과를 볼까요?

```bash
----------------------------------------------------------------
Benchmark                      Time             CPU   Iterations
----------------------------------------------------------------
upper_cost_of_pausing        384 ns          385 ns      1826598
```

이번에도 결과치가 뭔가 이상합니다. 간단한 로직인데, **400ns**에 가까운 결과가 나왔습니다. 이는 `state.PauseTiming()` 과 `state.ResumeTiming()`의 **오버헤드** 때문입니다. 따라서 이 함수들은 유용하지만, **마이크로-벤치마킹를 수행할때는 주의해서 사용**해야 합니다.

### 매개변수 전달

벤치마크를 수행할 때 **각 수행마다 매개변수를 전달**할 수 있습니다. 바로 예제 코드를 보겠습니다.

```cpp
static void args_getter(bm::State &state) {
  auto loop_count = static_cast<size_t>(state.range(0));
  auto add = static_cast<bool>(state.range(1));

  int a = std::rand(), b = std::rand(), c = 0;
  
  for (auto _ : state) {
    for (size_t i = 0; i != loop_count; ++i) {
      if (add)
        bm::DoNotOptimize(c += a + b);
      else
        bm::DoNotOptimize(c *= a * b);
    }
  }
}

BENCHMARK(args_getter)->Args({3, false})->Args({3, true});
BENCHMARK(args_getter)->Args({6, false})->Args({6, true});
```

`→Args()`를 통해 매개변수 리스트를 전달할 수 있으며, 함수 내부에서 `state.range()`로 전달받을 수 있습니다.

### 랜덤 인터리빙

GB에서는 **실행 순서나 반복(run) 간 편차가 큰** 벤치마크를 위해 **‘*랜덤 인터리빙’*** 기능을 제공합니다.

활성화 방법:

- 실행 시 플래그 `--benchmark_enable_random_interleaving=true` 추가
- (선택) 반복 횟수 지정: `--benchmark_repetitions=1`(`0`이 아닌 값)
- (선택) 각 반복의 최소 실행 시간 지정: `--benchmark_min_time=0.1`

**‘*랜덤 인터리빙’***은 **CPU 주파수가 순간적으로 상승, 하락하는 하드웨어**(e.g., Turbo Boost)에서 유용합니다. 벤치마크가 수행되는 도중 CPU가 2 GHz → 3 GHz로 부스트 되거나, 다시 하락하는 경우 결과값에 노이즈가 발생할 수 있습니다. 또한, 매우 큰 정적 배열에 접근하는 로직을 벤치마킹 하는 경우 **앞쪽 결과는 캐시 미스 때문에 느리고, 뒤쪽 결과는 캐시 히트 덕분에 빨라질 수 있습니다.**

이를 해결하기 위해 **‘*랜덤 인터리빙’*을 사용하면 다양한 케이스가 랜덤으로 섞여 실행되므로 이런 편차를 최소화 할 수 있습니다.**

## **하드웨어 성능 측정**

대부분의 CPU에는 **성능 카운터**(cycles, instructions 등)를 측정하는 기능이 있습니다. GB는 **libpmf(PMF)** 를 통해 이를 읽어 오는데, 이는 **리눅스 전용**이며 특정 커널에서만 지원됩니다.. WSL은 지원되지 않지만, pure 리눅스 환경이라면 sudo 권한과 옵션 한 줄이면 가능합니다.

```bash
sudo ./build_release/tutorial --benchmark_perf_counters="CYCLES,INSTRUCTIONS"
```

실제로는 GB를 **Linux perf** 와 조합해 쓰는 경우가 많습니다. perf 는 taskset 같은 옵션으로 **CPU 코어 마스크** 제어 등 추가 기능을 제공합니다.

```bash
$ sudo perf stat taskset 0xEFFFEFFFEFFFEFFFEFFFEFFFEFFFEFFF ./build_release/tutorial --benchmark_filter=super_sort

 Performance counter stats for 'taskset 0xEFFFEFFFEFFFEFFFEFFFEFFFEFFFEFFF ./build_release/tutorial --benchmark_filter=super_sort':

       23048674.55 msec task-clock                #   35.901 CPUs utilized          
           6627669      context-switches          #    0.288 K/sec                  
             75843      cpu-migrations            #    0.003 K/sec                  
         119085703      page-faults               #    0.005 M/sec                  
    91429892293048      cycles                    #    3.967 GHz                      (83.33%)
    13895432483288      stalled-cycles-frontend   #   15.20% frontend cycles idle     (83.33%)
     3277370121317      stalled-cycles-backend    #    3.58% backend cycles idle      (83.33%)
    16689799241313      instructions              #    0.18  insn per cycle         
                                                  #    0.83  stalled cycles per insn  (83.33%)
     3413731599819      branches                  #  148.110 M/sec                    (83.33%)
       11861890556      branch-misses             #    0.35% of all branches          (83.34%)

     642.008618457 seconds time elapsed

   21779.611381000 seconds user
    1244.984080000 seconds sys

```

# 마치며…

GB는 강력한 벤치마킹 라이브러리 입니다. 그러나, 올바른 사용법을 알지 못하면 측정 결과에 노이즈가 끼게 되고, 불완전한 결과치를 얻을 수 있습니다. 따라서 사용법와 주의사항을 숙지한 뒤 올바르게 사용하기를 바랍니다.