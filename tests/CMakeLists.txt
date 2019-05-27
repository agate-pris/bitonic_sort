set(target foo-tests)
add_executable(${target}
    foo.cpp
)
add_test(foo "${EXECUTABLE_OUTPUT_PATH}/${target}")
