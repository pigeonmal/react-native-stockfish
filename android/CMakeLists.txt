project(NitroStockfish)
cmake_minimum_required(VERSION 3.9.0)

set (PACKAGE_NAME NitroStockfish)
set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 20)

# Define C++ library and add all sources
add_library(${PACKAGE_NAME} SHARED
        src/main/cpp/cpp-adapter.cpp
        ../cpp/HybridStockfishEngine.cpp
        ../cpp/HybridStockfishFactory.cpp
        ../cpp/StockfishConversions.cpp
        ../third-party/stockfish/src/benchmark.cpp
        ../third-party/stockfish/src/bitboard.cpp
        ../third-party/stockfish/src/engine.cpp
        ../third-party/stockfish/src/evaluate.cpp
        ../third-party/stockfish/src/memory.cpp
        ../third-party/stockfish/src/misc.cpp
        ../third-party/stockfish/src/movegen.cpp
        ../third-party/stockfish/src/movepick.cpp
        ../third-party/stockfish/src/position.cpp
        ../third-party/stockfish/src/score.cpp
        ../third-party/stockfish/src/search.cpp
        ../third-party/stockfish/src/thread.cpp
        ../third-party/stockfish/src/timeman.cpp
        ../third-party/stockfish/src/tt.cpp
        ../third-party/stockfish/src/tune.cpp
        ../third-party/stockfish/src/uci.cpp
        ../third-party/stockfish/src/ucioption.cpp
        ../third-party/stockfish/src/syzygy/tbprobe.cpp
        ../third-party/stockfish/src/nnue/features/full_threats.cpp
        ../third-party/stockfish/src/nnue/features/half_ka_v2_hm.cpp
        ../third-party/stockfish/src/nnue/network.cpp
        ../third-party/stockfish/src/nnue/nnue_accumulator.cpp
        ../third-party/stockfish/src/nnue/nnue_misc.cpp
)

# Add Nitrogen specs :)
include(${CMAKE_SOURCE_DIR}/../nitrogen/generated/android/NitroStockfish+autolinking.cmake)

# Set up local includes
include_directories(
        "src/main/cpp"
        "../cpp"
        "../third-party/stockfish/src"
)

find_library(LOG_LIB log)

# Link all libraries together
target_link_libraries(
        ${PACKAGE_NAME}
        ${LOG_LIB}
        android                                   # <-- Android core
)

target_compile_definitions(${PACKAGE_NAME} PRIVATE NDEBUG USE_PTHREADS NO_PREFETCH)
target_compile_options(${PACKAGE_NAME} PRIVATE
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Debug>:-O1>
        $<$<COMPILE_LANGUAGE:CXX>:-Wa,-I${CMAKE_CURRENT_SOURCE_DIR}/../third-party/stockfish/src>
        -ffunction-sections
        -fdata-sections
)
