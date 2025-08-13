export const featureCards = [
  {
    url: "https://bayernmuller.github.io/blog/250809-10x-faster-cpp-string-split/",
    date: "2025-08-08",
    author: "BayernMuller"
  },
  {
    url: "https://bayernmuller.github.io/blog/250802-what-wrong-with-std-string/",
    date: "2025-08-01",
    author: "BayernMuller"
  },
  {
    url: "https://jin-uu.github.io/blog/250801-fast-argument-parsing/",
    date: "2025-08-01",
    author: "jin-uu"
  },
  {
    url: "https://bayernmuller.github.io/blog/250719-javascript-vector-cosine-distance/",
    date: "2025-07-19",
    author: "BayernMuller"
  },
  {
    url: "https://jin-uu.github.io/blog/250719-fast-cosine-similarity/",
    date: "2025-07-19",
    author: "jin-uu"
  },
  {
    url: "https://bayernmuller.github.io/blog/250712-string-and-simd/",
    date: "2025-07-12",
    author: "BayernMuller"
  },
  {
    url: "https://bayernmuller.github.io/blog/250705-simd-instruction/",
    date: "2025-07-05",
    author: "BayernMuller"
  }
];

// 최신 순으로 정렬하는 함수
export const getLatestFeatureCards = (limit = 6) => {
  return featureCards
    .sort((a, b) => new Date(b.date) - new Date(a.date))
    .slice(0, limit);
};

// 특정 작성자의 카드만 가져오는 함수
export const getFeatureCardsByAuthor = (author) => {
  return featureCards.filter(card => card.author === author);
}; 