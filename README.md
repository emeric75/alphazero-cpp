# Alphazero-cpp

A generic/easily extendable for multiple games implementation of Alphazero.
The library centers around 4 main concepts
- `State` : describes a particular state of a game, and the achievable states accessible with one move/action
- `Tree` : used to store additional metadata on a state (total number of wins starting from state, ...)
- `Player` :  an agent playing a game, can use a tree in order to choose which action to take
- `InitPredictor` : encapsulates the neural network logic used by the Alphazero players

To create an Alphazero agent on a new game,
- implement a new `State` corresponding to your game
- implement an `ÌnitPredictor` which, given a state of your game, returns a starting probability distribution on the possible action at this state
- you will then be able to initialize an `MCTreePUCTPlayer`, with the inital state of your game, and the init predictor

Basic `State` and `InitPredictor` implementations are given respectively in `src/state` and `src/init_predictor`
`src/MCTreePUCTTesting.cpp` is an example training file for an Alphazero agent (vs a brute force agent) on TicTacToe.

`libtorch` (the C++ frontend) is used for the neural network support for `InitPredictor`.

## Build
`CMake` is used here to automate the build process.
You must have `libtorch` installed ([link to download page](https://pytorch.org/get-started/locally/)) in order to build the sources (unzip the downloaded file and remember its absolute path on your file system, suppose it is `<TORCHPATH>`).

With `CMake` installed, run the following commands

```
    cmake -DCMAKE_PREFIX_PATH=<TORCHPATH> -B build -S src
    cmake --build build
```

This will produce 6 executables (`mctreetesting`, `playertesting`, `gametesting`, `torchtesting`, `initpredictortesting`, `mctreepucttesting`) in the directory `build/target`, testing various aspects of the project.
