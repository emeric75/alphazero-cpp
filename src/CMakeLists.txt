cmake_minimum_required(VERSION 3.26)
project(alphazero)

find_package(Torch REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

link_libraries("${TORCH_LIBRARIES}")
include_directories(tree state player game init_predictor)

add_executable(mctreetesting MCTreeTesting.cpp tree/MCTree.cpp state/TicTacToeState.cpp)
set_property(TARGET mctreetesting PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")

add_executable(playertesting PlayerTesting.cpp player/BruteForcePlayer.cpp tree/MCTree.cpp state/TicTacToeState.cpp)
set_property(TARGET playertesting PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")

add_executable(gametesting GameTesting.cpp player/BruteForcePlayer.cpp player/MCTreePlayer.cpp tree/MCTree.cpp state/TicTacToeState.cpp game/Game.cpp)
set_property(TARGET gametesting PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")

add_executable(torchtesting TorchTesting.cpp)
target_link_libraries(torchtesting "${TORCH_LIBRARIES}")
set_property(TARGET torchtesting PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")

add_executable(initpredictortesting InitPredictorTesting.cpp state/TicTacToeState.cpp)
target_link_libraries(initpredictortesting "${TORCH_LIBRARIES}")
set_property(TARGET initpredictortesting PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")

add_executable(mctreepucttesting MCTreePUCTTesting.cpp tree/MCTreePUCT.cpp player/MCTreePUCTPlayer.cpp tree/MCTree.cpp player/BruteForcePlayer.cpp state/TicTacToeState.cpp game/Game.cpp)
target_link_libraries(mctreepucttesting "${TORCH_LIBRARIES}")
set_property(TARGET mctreepucttesting PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target")