project(monkey)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 设置要编译的头文件
set(HEADER_FILES 
    ./include/include.h
    ./utils/timer.h
    )

# 设置要编译的源文件
set(SOURCE_FILES 
    main.cpp
    ./lexer/lexer.cpp
    ./parser/parser.cpp
    ./builtins/builtins.cpp
    ./repl/repl.cpp
    ./code/code.cpp
    ./compiler/compiler.cpp
    ./vm/vm.cpp
    ./symbol/symbol.cpp
    )

# 生成可执行文件
add_executable(monkey ${SOURCE_FILES} ${HEADER_FILES})