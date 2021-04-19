cmake_minimum_required (VERSION 3.8)

project ("pegas_tray")

add_executable (pegas_tray "main.c" "pegas_tray.h")

target_link_libraries(pegas_tray wsock32 ws2_32)