#!/bin/bash
set -e

SOURCE_DIR="$1"

# Safety check
if [ -z "$SOURCE_DIR" ] || [ ! -d "$SOURCE_DIR" ]; then
    echo "Error: valid source directory required"
    exit 1
fi

# Escape slashes for sed substitution
ESCAPED_SOURCE_DIR=$(echo "$SOURCE_DIR" | sed 's_/_\\/_g')

# Replace ${CMAKE_SOURCE_DIR} with the actual source dir path
find "$SOURCE_DIR" -type f \( -name "CMakeLists.txt" -or -name "*.cmake" \) \
    -exec sed -i "s/\${CMAKE_SOURCE_DIR}/${ESCAPED_SOURCE_DIR}/g" {} \;

echo "Patched and replaced CMAKE_SOURCE_DIR with $SOURCE_DIR"
