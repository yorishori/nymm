#!/bin/bash

rm -rf bin
mkdir -p bin/db
# Build the backend and add it to bin
pushd backend
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j$(nproc) 
popd
cp backend/build/backend bin/backend
cp backend/config.json bin/config.json

[[ -r filename ]] && ln -s backend/build/compile_commands.json compile_commands.json

# Build the front end and add it to bin
pushd frontend
npm ci
npm run build
popd
cp -r frontend/dist bin/
