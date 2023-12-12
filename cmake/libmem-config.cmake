set(LIBMEM_DOWNLOAD_URL "https://github.com/nathan818fr/libmem/releases/download/{{version}}/libmem-{{version}}-{{target}}.tar.gz")
set(LIBMEM_DOWNLOAD_VERSION "4.4.0-nathan818.0")

include(FetchContent)
fetchcontent_declare(libmem-config URL "https://raw.githubusercontent.com/nathan818fr/libmem/config-v1/libmem-config.cmake" DOWNLOAD_NO_EXTRACT TRUE)
fetchcontent_makeavailable(libmem-config)
include("${libmem-config_SOURCE_DIR}/libmem-config.cmake")
