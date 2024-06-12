#!/usr/bin/bash
cmake --preset Emscripten -S . -B ./web-build &&
  cd web-build/ &&
  cmake -E time ninja -j8 &&
  /home/henry/emsdk/upstream/emscripten/emrun SokobanGameDemo.html
