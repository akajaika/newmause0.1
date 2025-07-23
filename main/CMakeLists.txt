# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)


idf_component_register(
  SRCS
    "main.cpp"
    "adc.cpp"
    "gpio_setup.cpp"
    "init.cpp"
    "interrupt.cpp"
    "MA732.cpp"
    "mause.cpp"
    "motor.cpp"
    "MPU6500.cpp"
    "PCA9533.cpp"
    "run.cpp"
    "spi.cpp"
    "test.cpp"
  INCLUDE_DIRS
    "."  # main/ にあるヘッダはここを指定
)
