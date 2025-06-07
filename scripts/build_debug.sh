#!/bin/bash

set -e

BUILD_DIR="build/debug"

echo "Configuring CMake in Debug mode..."
cmake -B "$BUILD_DIR" -S . \
	-DCMAKE_BUILD_TYPE=Debug \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "Building project..."
cmake --build "$BUILD_DIR"

echo "Linking compile_commands.json..."
ln -sf "$BUILD_DIR/compile_commands.json" ./compile_commands.json

SHADER_DIR="src/shaders"
if [ -d "$SHADER_DIR" ]; then
  echo "Navigating to $SHADER_DIR and running compile.sh"
  cd "$SHADER_DIR" || exit
  ./compile.sh || { echo "Shader compilation failed"; exit 1; }
else
  echo "Shader directory $SHADER_DIR does not exist. Skipping shader compilation."
fi

echo "Done."
