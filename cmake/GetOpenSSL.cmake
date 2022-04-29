# Provides dependency openssl
find_package(OpenSSL QUIET)
if(NOT OPENSSL_FOUND)
    add_subdirectory(
        ${CMAKE_SOURCE_DIR}/3rd/openssl
        ${CMAKE_CURRENT_BINARY_DIR}/openssl
        EXCLUDE_FROM_ALL
    )
    set(OPENSSL_SSL_LIBRARY ssl)
    set(OPENSSL_CRYPTO_LIBRARY crypto)
    set(OPENSSL_INCLUDE_DIR "${openssl_BINARY_DIR}/include" "${openssl_BINARY_DIR}")
    set(OPENSSL_FOUND ON)
    message(STATUS "Build OpenSSL: ${openssl_BINARY_DIR}")
endif()

set (HAVE_OPENSSL 1)
include_directories (${OPENSSL_INCLUDE_DIR})

#target_link_libraries (native-lib  ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
