cmake_minimum_required(VERSION 3.16)
# project(PlutoSDR CXX)

# set(CMAKE_BUILD_TYPE Debug)

# # Ищем библиотеку SoapySDR
# find_package(SoapySDR REQUIRED)

# set(SDR_SOURCE_FILES
#     src/sdr/main.cpp
# )

file(GLOB_RECURSE RX_TX_test
    "src/TX/*.cpp"
    "src/RX/*.cpp"
    "tests/TX_RX_test.cpp"
)


# Добавляем исполняемый файл
add_executable(RX_TX_test ${RX_TX_test})
# add_executable(SDR ${SDR_SOURCE_FILES})

# Линкуем библиотеки к исполняемому файлу
# target_link_libraries(SDR ${SoapySDR_LIBRARIES})
