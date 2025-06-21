#!/bin/bash

set -e

ASTRO_ASSETS_PATH="astro/src/content/blog/assets"
ASTRO_BLOG_PATH="astro/src/content/blog"
ASTRO_THUMBNAIL_PATH="astro/public/thumbnails"

BLOG_PATH="posts"
BLOG_ASSETS_PATH="$BLOG_PATH/assets"
BLOG_THUMBNAIL_PATH="$BLOG_PATH/thumbnails"

# Copy blog posts to Astro blog path
rm -rf $ASTRO_BLOG_PATH
mkdir -p $ASTRO_BLOG_PATH
find $BLOG_PATH -type f -name '*.md' -exec cp {} $ASTRO_BLOG_PATH \;

# Copy assets to Astro assets path
if [ -d "$BLOG_ASSETS_PATH" ]; then
    rm -rf $ASTRO_ASSETS_PATH
    cp -r $BLOG_ASSETS_PATH $ASTRO_ASSETS_PATH
fi

# Copy thumbnail to Astro assets path
if [ -d "$BLOG_THUMBNAIL_PATH" ]; then
    rm -rf $ASTRO_THUMBNAIL_PATH
    cp -r $BLOG_THUMBNAIL_PATH $ASTRO_THUMBNAIL_PATH
fi
