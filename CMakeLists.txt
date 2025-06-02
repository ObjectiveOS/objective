cmake_minimum_required(VERSION 3.16)
project(objectiveOS LANGUAGES C CXX ASM_NASM)

set(CMAKE_VERBOSE_MAKEFILE ON)

include(toolchain.cmake)
enable_language(ASM_NASM)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/arch
    ${CMAKE_SOURCE_DIR}
)
file(GLOB_RECURSE CXX_SOURCES
    "*.h"
    "*.hpp" 
    "*.cpp"
    EXCLUDE "CMakeFiles/*"

)
file(GLOB_RECURSE C_SOURCES 
    "*.c"
    EXCLUDE "CMakeFiles/*"

)

file(GLOB_RECURSE ASM_NASM_SOURCES
    "kernel/*.s"
    "arch/x86/*.s"
    "arch/x86/*.S"
    EXCLUDE "CMakeFiles/*"

)

set_source_files_properties(${ASM_NASM_SOURCES} PROPERTIES LANGUAGE ASM_NASM)

add_executable(kernel 
    ${ASM_NASM_SOURCES}
    ${CXX_SOURCES}   
    ${C_SOURCES}
)

set(CMAKE_CXX_LINK_EXECUTABLE 
    "<CMAKE_LINKER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>"
)

execute_process(
    COMMAND sh -c "i686-elf-gcc -print-libgcc-file-name"
    OUTPUT_VARIABLE LIBGCC_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

get_filename_component(LIBGCC_DIR ${LIBGCC_PATH} DIRECTORY)
target_compile_options(kernel PRIVATE
    
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++20>
    $<$<COMPILE_LANGUAGE:CXX>:-m32>
    $<$<COMPILE_LANGUAGE:CXX>:-ffreestanding>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-nostdlib>
    $<$<COMPILE_LANGUAGE:CXX>:-g>
)

target_link_options(kernel PRIVATE
    -T${CMAKE_SOURCE_DIR}/kernel/linker.ld
    -nostdlib
    -static
    --nmagic
    -m elf_i386
    -L${LIBGCC_DIR}
    -lgcc
)

add_custom_target(iso ALL
    COMMAND mkdir -p iso/os
    COMMAND mkdir -p iso/boot/grub

    COMMAND cp ${CMAKE_BINARY_DIR}/kernel iso/os/kernel.bin
    COMMAND cp ${CMAKE_SOURCE_DIR}/iso/boot/grub/grub.cfg iso/boot/grub/

    COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/objectiveOS.iso ${CMAKE_BINARY_DIR}/iso
    COMMENT "Creating ISO image"
    DEPENDS kernel
)

add_custom_target(run
    COMMAND qemu-system-i386 -cdrom ${CMAKE_BINARY_DIR}/objectiveOS.iso -vga std
    COMMENT "Starting QEMU emulator"
    DEPENDS iso
)

add_custom_target(debug
    COMMAND qemu-system-i386 -cdrom ${CMAKE_BINARY_DIR}/objectiveOS.iso -vga std -S -gdb tcp::1234
    COMMENT "Starting QEMU in debug mode (GDB server: 1234)"
    DEPENDS iso
)

add_custom_target(gdb
    COMMAND gdb -ex "target remote :1234" -ex "break kmain" ${CMAKE_BINARY_DIR}/kernel
    COMMENT "Connecting GDB to QEMU"
)

