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

echo "Done."
