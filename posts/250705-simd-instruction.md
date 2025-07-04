---
title: 'SIMD Instruction'
description: 'Single Instruction, Multiple Data.'
pubDate: 'Jul 05 2025'
heroImage: 'simd.png'
categories: ['post']
author: 'BayernMuller'
tags: ['SIMD', 'NEON', 'CPU', 'Performance']
---

Python 에서 사용되는 대표적인 라이브러리로 `numpy` 를 꼽을 수 있습니다. `numpy` 는 벡터화된 연산을 지원합니다. 배열/행렬 끼리의 덧셈, 곱셈 등의 연산을 아주 빠르게 수행할 수 있습니다.

`numpy` 는 C/C++ 로 구현되어 있습니다. 그래서 느린 Python 을 극복하고 빠르게 동작할 수 있죠.

그럼 만약 우리가 `numpy` 처럼 행렬 연산을 직접 구현한다면 어떻게 해야 할까요?

```cpp
void matrix_multiply(const float* a, const float* b, float* c, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            c[i * n + j] = 0;
            for (int k = 0; k < n; ++k) {
                c[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }
}
```

직관적으로 위와같은 코드를 생각해볼 수 있습니다.

이 코드는 작은 행렬에 대해서는 Python 이나 Javascript 같은 스크립트 언어들보다 훨씬 빠르게 동작합니다.

하지만 큰 행렬에 대해서는 어떨까요? 두 행렬의 크기를 *1024x1024* 로 설정하고 `numpy` 와 비교해보겠습니다.

```python
def matrix_multiply(a: np.ndarray, b: np.ndarray) -> np.ndarray
    return np.dot(a, b)
```

```bash
❯ g++ -O3 matmul.cpp -std=c++17 -o matmul
❯ ./matmul
Time taken: 1.3178 seconds

❯ python3 matmul.py
Time taken: 0.0216 seconds
```

동일한 행렬곱 연산을 수행하는데 `-O3` 최적화 레벨을 적용한 C++ 코드보다 `numpy` 의 Python 구현이 더 훨씬 더 빠릅니다. 어떻게 이게 가능할까요?

## SIMD

정답은 SIMD 에 있습니다.

SIMD 는 Single Instruction Multiple Data 의 약자로, 하나의 명령어로 여러 개의 데이터를 동시에 처리하는 CPU 명령어 집합을 말합니다.

즉 이 SIMD은 CPU 차원에서 지원하는 기능이며, CPU 의 종류별로 지원 여부가 갈리며 지원하는 범위, 사용하는 방법이 모두 다릅니다.



