#!/bin/bash

set -e

ASTRO_BLOG_PATH="astro/src/content/blog"
BLOG_PATH="posts"

ASTRO_ASSETS_PATH="astro/public/assets"
ASSETS_PATH="assets"

# Copy blog posts to Astro blog path
rm -rf $ASTRO_BLOG_PATH
mkdir -p $ASTRO_BLOG_PATH
find $BLOG_PATH -type f -name '*.md' -exec cp {} "$ASTRO_BLOG_PATH" \;

# Copy assets to Astro assets path
rm -rf $ASTRO_ASSETS_PATH
cp -r $ASSETS_PATH $ASTRO_ASSETS_PATH
    
# Replace /assets with assets
if [[ "$OSTYPE" == "darwin"* ]]; then
    find $ASTRO_BLOG_PATH -type f -name '*.md' -exec sed -i '' 's/!\[\](\/assets/!\[\](\/public\/assets/g' {} +
else
    find $ASTRO_BLOG_PATH -type f -name '*.md' -exec sed -i 's/!\[\](\/assets/!\[\](\/public\/assets/g' {} +
fi
