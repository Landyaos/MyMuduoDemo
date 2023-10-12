#!/bin/bash

# 构建目录
BUILD_DIR="build"

# 清理构建目录
clean() {
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "Done."
}

# 编译项目
build() {
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || exit

    echo "Running CMake..."
    cmake ..
    
    echo "Building project..."
    make
    echo "Build completed. Executable is in $BUILD_DIR"
}

# 主函数
main() {
    case "$1" in
        clean)
            clean
            ;;
        *)
            build
            ;;
    esac
}

main "$@"
