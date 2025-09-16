cmake_minimum_required(VERSION 3.15)
project(WordleGame LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(wordle
    src/main.cpp
    src/wordle.cpp
    src/bot.cpp
)

# Point targets at your headers folder
target_include_directories(wordle PRIVATE ${CMAKE_SOURCE_DIR}/headers)

# (optional, helps IntelliSense)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
