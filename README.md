# Wordle Information Theory (C++)

A C++17 implementation of the classic **Wordle** game — with both **interactive play** and an **auto-playing bot** that uses information-theory heuristics to suggest optimal guesses.  

The project is structured to be clean and modular, using **CMake** for building, and tested with **MSYS2 MinGW (UCRT64)** on Windows and `g++`.

---

## Building the Project

### Prerequisites
- **C++17 compiler** (g++ / clang / MSVC)
- [**CMake**](https://cmake.org/download/) (≥ 3.15)
- On Windows: [**MSYS2**](https://www.msys2.org/) with UCRT64 toolchain recommended

### Build Instructions
From the project root:

```bash
# Create build folder
mkdir build
cd build

# Configure with CMake
cmake -G "MinGW Makefiles" ..

# Build
cmake --build .
```

The resulting executable will be at:
```
build/wordle.exe
```

---

## Running the Game

From project root (so the `data/` folder is found correctly):

```bash
./build/wordle.exe
```

### Modes
You will be prompted for game mode:
- **Normal Mode** → Play interactively, like the real Wordle
- **Auto Play** → Let the bot play automatically
---

## The Bot

The **WordleBot** uses an **information theory approach**:
- Computes **feedback patterns** (green/yellow/gray)
- Eliminates impossible candidates
- Scores remaining guesses by expected information gain
- Always selects the guess that minimizes average remaining possibilities

This allows it to play optimally and even test different starting words.

---

## VS Code Integration

If you use VS Code:
1. Install the **CMake Tools** extension
2. Open the repo
3. Configure CMake (`Ctrl+Shift+P` → *CMake: Configure*)
4. Use the bottom status bar to **Build** and **Run**

A provided `.vscode/launch.json` makes debugging with GDB easy.  

---

MIT License.
