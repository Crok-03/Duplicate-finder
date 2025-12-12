cmake_minimum_required(VERSION 3.16)

project(DuplicateFinder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ---- Собираем файлы ----
file(GLOB CORE_SRC "src/core/*.cpp")
file(GLOB UI_SRC "src/ui/*.cpp" "src/ui/WizardPages/*.cpp")

set(SRC
    src/main.cpp
    resources.qrc      # ресурс с картинками
    ${CORE_SRC}
    ${UI_SRC}
)

# ---- Добавляем exe как GUI (без консоли) ----
add_executable(${PROJECT_NAME} WIN32
    ${SRC}
    appicon.rc         # иконка для EXE
)

# ---- Пути к заголовкам ----
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/ui/WizardPages
)

# ---- Qt ----
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
