# Build the backend and add it to bin
cd backend
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j$(nproc) 
cd ..
mkdir -p bin
rm -f bin/backend
cp backend/build/backend bin/backend
rm -f bin/config.json
cp backend/config.json bin/config.json

[[ -r filename ]] && ln -s backend/build/compile_commands.json compile_commands.json

# Build the front end and add it to bin
# this will be react, for now just copy a simple index.html
mkdir -p bin/dist
cp frontend/index.html bin/dist/index.html
