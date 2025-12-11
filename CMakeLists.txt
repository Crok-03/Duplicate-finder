cmake_minimum_required(VERSION 3.16)

project(DuplicateFinder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 через MSYS2
set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Собираем списки исходников
file(GLOB CORE_SRC
    "src/core/*.cpp"
)

file(GLOB UI_SRC
    "src/ui/*.cpp"
    "src/ui/WizardPages/*.cpp"
)

set(SRC
    src/main.cpp
    ${CORE_SRC}
    ${UI_SRC}
)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/WizardPages
        ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# Если используешь заголовки в include/, можно явно добавить:
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
