cmake_minimum_required(VERSION 3.16)
# project(PlutoSDR CXX)

# set(CMAKE_BUILD_TYPE Debug)

# # Ищем библиотеку SoapySDR
# find_package(SoapySDR REQUIRED)

# set(SDR_SOURCE_FILES
#     src/sdr/main.cpp
# )

set(CMAKE_BUILD_TYPE Debug)

file(GLOB_RECURSE TX_test
    "src/TX/*.cpp"
    "src/general/*.cpp"
    "tests/TX_test.cpp"
)

file(GLOB_RECURSE RX_test
    "src/RX/*.cpp"
    "src/TX/*.cpp"
    "src/general/*.cpp"
    "tests/RX_test.cpp"
)


# Добавляем исполняемый файл
add_executable(TX_test ${TX_test})
add_executable(RX_test ${RX_test})
# add_executable(SDR ${SDR_SOURCE_FILES})

# Линкуем библиотеки к исполняемому файлу
# target_link_libraries(SDR ${SoapySDR_LIBRARIES})
