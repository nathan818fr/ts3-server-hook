set(ASIO_ROOT
    "${PROJECT_SOURCE_DIR}/external/asio/asio"
    CACHE PATH "Path to asio source directory")

add_library(asio STATIC "${ASIO_ROOT}/src/asio.cpp")
add_library(asio::asio ALIAS asio)

target_compile_features(asio PUBLIC cxx_std_11)
target_compile_definitions(asio PUBLIC -DASIO_STANDALONE -DASIO_SEPARATE_COMPILATION -DASIO_NO_DEPRECATED)
set_target_properties(asio PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(asio PUBLIC "${ASIO_ROOT}/include")
target_link_libraries(asio PRIVATE Threads::Threads)
