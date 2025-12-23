cmake_minimum_required(VERSION 3.16)
project(PlutoSDR CXX)

set(CMAKE_BUILD_TYPE Debug)

# Ищем библиотеку SoapySDR
find_package(SoapySDR REQUIRED)

set(SDR_SOURCE_FILES
    src/sdr/main.cpp
)

file(GLOB_RECURSE TRANSMITTER_SOURCE_FILES
    "src/TX/*.cpp"
    "tests/TX_test.cpp"
)

file(GLOB_RECURSE RECEIVER_SOURCE_FILES
    "src/RX/*.cpp"
    "tests/RX_test.cpp"
)

# Добавляем исполняемый файл
add_executable(TX ${TRANSMITTER_SOURCE_FILES})
add_executable(RX ${RECEIVER_SOURCE_FILES})
add_executable(SDR ${SDR_SOURCE_FILES})

# Линкуем библиотеки к исполняемому файлу
target_link_libraries(SDR ${SoapySDR_LIBRARIES})
