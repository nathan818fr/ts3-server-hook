set(LIBMEM_ROOT
    "${PROJECT_SOURCE_DIR}/external/libmem"
    CACHE PATH "Path to libmem root directory")

find_path(LIBMEM_INCLUDE_DIR
    NAMES libmem/libmem.h
    PATHS "${LIBMEM_ROOT}"
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
)

find_library(LIBMEM_LIBRARY
    NAMES liblibmem.a libmem.lib
    PATHS "${LIBMEM_ROOT}"
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
)

find_library(LIBMEM_CAPSTONE_LIBRARY
    NAMES libcapstone.a capstone.lib
    PATHS "${LIBMEM_ROOT}"
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
)

find_library(LIBMEM_KEYSTONE_LIBRARY
    NAMES libkeystone.a keystone.lib
    PATHS "${LIBMEM_ROOT}"
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
)

find_library(LIBMEM_LIEF_LIBRARY
    NAMES libLIEF.a LIEF.lib
    PATHS "${LIBMEM_ROOT}"
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
)

find_library(LIBMEM_LLVM_LIBRARY
    NAMES libllvm.a llvm.lib
    PATHS "${LIBMEM_ROOT}"
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libmem
    DEFAULT_MSG
    LIBMEM_LIBRARY
    LIBMEM_INCLUDE_DIR
    LIBMEM_CAPSTONE_LIBRARY
    LIBMEM_KEYSTONE_LIBRARY
    LIBMEM_LIEF_LIBRARY
    LIBMEM_LLVM_LIBRARY
)

if (LIBMEM_FOUND)
  set(LIBMEM_INCLUDE_DIRS "${LIBMEM_INCLUDE_DIR}")
  set(LIBMEM_LIBRARIES
      "${LIBMEM_LIBRARY}"
      "${LIBMEM_CAPSTONE_LIBRARY}"
      "${LIBMEM_KEYSTONE_LIBRARY}"
      "${LIBMEM_LIEF_LIBRARY}"
      "${LIBMEM_LLVM_LIBRARY}"
  )
endif ()

mark_as_advanced(
    LIBMEM_INCLUDE_DIR
    LIBMEM_LIBRARY
    LIBMEM_CAPSTONE_LIBRARY
    LIBMEM_KEYSTONE_LIBRARY
    LIBMEM_LIEF_LIBRARY
    LIBMEM_LLVM_LIBRARY
)

add_library(libmem INTERFACE)
add_library(libmem::libmem ALIAS libmem)

target_include_directories(libmem INTERFACE "${LIBMEM_INCLUDE_DIRS}")
target_link_libraries(libmem INTERFACE "${LIBMEM_LIBRARIES}")
