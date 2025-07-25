export const featureCards = [
  {
    url: "https://bayernmuller.github.io/blog/250719-javascript-vector-cosine-distance/",
    date: "2025-07-19",
    author: "BayernMuller"
  },
  {
    url: "https://github.com/withastro/astro",
    date: "2024-01-15",
    author: "astro"
  },
  {
    url: "https://github.com/tailwindlabs/tailwindcss",
    date: "2024-01-10",
    author: "tailwindlabs"
  },
  {
    url: "https://github.com/vuejs/vue",
    date: "2024-01-08",
    author: "vuejs"
  },
  {
    url: "https://github.com/facebook/react",
    date: "2024-01-05",
    author: "facebook"
  },
  {
    url: "https://github.com/nodejs/node",
    date: "2024-01-03",
    author: "nodejs"
  },
  {
    url: "https://astro.build/blog/astro-4/",
    date: "2023-12-05",
    author: "astro"
  },
  {
    url: "https://tailwindcss.com/blog/tailwindcss-v3-4",
    date: "2023-12-10",
    author: "tailwindlabs"
  },
  {
    url: "https://reactjs.org/blog/2023/03/16/introducing-react-dev.html",
    date: "2023-03-16",
    author: "facebook"
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