# Compile

 mkdir build-web
 cd build-web
 cmake .. -DCMAKE_TOOLCHAIN_FILE=${EMSCRIPTEN}/cmake/Modules/Platform/Emscripten.cmake
 make clean all -j4

# Run

 python -m SimpleHTTPServer
 http://localhost:8000/index.html

