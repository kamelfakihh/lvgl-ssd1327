cmake_minimum_required(VERSION 3.14)
include(FetchContent)
project(lvgl-demo LANGUAGES C CXX)

set(OLED_DIR_Config ${CMAKE_CURRENT_SOURCE_DIR}/src/oled-lib/Config)
set(OLED_DIR_OLED   ${CMAKE_CURRENT_SOURCE_DIR}/src/oled-lib/OLED)
set(OLED_DIR_FONTS  ${CMAKE_CURRENT_SOURCE_DIR}/src/oled-lib/Fonts)
set(OLED_DIR_GUI    ${CMAKE_CURRENT_SOURCE_DIR}/src/oled-lib/GUI)

file(GLOB OLED_LIB_SRC
    "${OLED_DIR_OLED}/*.c"
    "${OLED_DIR_Config}/*.c"
    "${OLED_DIR_GUI}/*.c"
    "${OLED_DIR_FONTS}/*.c"
)

# Choose library based on USELIB
set(USELIB "USE_DEV_LIB")
if(USELIB STREQUAL "USE_BCM2835_LIB")
    set(LIBS bcm2835 m)
elseif(USELIB STREQUAL "USE_WIRINGPI_LIB")
    set(LIBS wiringPi m)
elseif(USELIB STREQUAL "USE_DEV_LIB")
    set(LIBS lgpio m)
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -D${USELIB}")

# Build an executable called "lvgl-demo"
add_executable(lvgl-demo src/main.c)

# Specify path to own LVGL config header
set(LV_CONF_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_conf.h
    CACHE STRING "" FORCE)

# Fetch LVGL from GitHub
FetchContent_Declare(lvgl GIT_REPOSITORY https://github.com/lvgl/lvgl.git GIT_TAG release/v8.3)
FetchContent_MakeAvailable(lvgl)

# The target "lvgl-demo" depends on LVGL
target_include_directories(lvgl-demo PRIVATE ${lvgl_SOURCE_DIR} ${lvgl_BINARY_DIR})
target_link_libraries(lvgl-demo PRIVATE lvgl::lvgl)

# Include OLED libraries
target_include_directories(lvgl-demo PRIVATE ${OLED_DIR_Config} ${OLED_DIR_GUI} ${OLED_DIR_OLED})
target_sources(lvgl-demo PRIVATE ${OLED_LIB_SRC})
target_link_libraries(lvgl-demo PRIVATE ${LIBS})