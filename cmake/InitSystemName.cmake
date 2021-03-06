#include(haiUtil)

#string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSNAME_LC)
#set_if(DARWIN	   (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#    OR (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
#    OR (${CMAKE_SYSTEM_NAME} MATCHES "tvOS")
#    OR (${CMAKE_SYSTEM_NAME} MATCHES "watchOS"))
#set_if(LINUX       ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#set_if(BSD         ${SYSNAME_LC} MATCHES "bsd$")
#set_if(MICROSOFT   WIN32 AND (NOT MINGW AND NOT CYGWIN))
#set_if(GNU         ${CMAKE_SYSTEM_NAME} MATCHES "GNU")
#set_if(ANDROID     ${SYSNAME_LC} MATCHES "android")
#set_if(SUNOS       "${SYSNAME_LC}" MATCHES "sunos")
#set_if(POSIX       LINUX OR DARWIN OR BSD OR SUNOS OR ANDROID OR (CYGWIN AND CYGWIN_USE_POSIX))
#set_if(SYMLINKABLE LINUX OR DARWIN OR BSD OR SUNOS OR CYGWIN OR GNU)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(Windows TRUE)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(iOS TRUE)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(Linux TRUE)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(macOS TRUE)
endif()
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(Android TRUE)
endif()
if(iOS OR Linux OR macOS OR Android)
    set(POSIX TRUE)
endif()
