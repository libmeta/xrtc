include(FetchContent)

set(DEP_NAME webrtc)
set(${DEP_NAME}_URL "")
set(${DEP_NAME}_NAME ${DEP_NAME})

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(${DEP_NAME}_NAME ${DEP_NAME}_android)
    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-android.zip")
elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(${DEP_NAME}_NAME ${DEP_NAME}_ios)
    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-ios.zip")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(${DEP_NAME}_NAME ${DEP_NAME}_win)
    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-win.zip")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(${DEP_NAME}_NAME ${DEP_NAME}_macos)
    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-mac.zip")
endif()

FetchContent_Declare( ${${DEP_NAME}_NAME}
    URL     ${${DEP_NAME}_URL})

FetchContent_MakeAvailable(${${DEP_NAME}_NAME})

set(WEBRTC_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/${${DEP_NAME}_NAME}-src )


#set(DEP_NAME webrtc)
#set(${DEP_NAME}_URL "")
#set(${DEP_NAME}_NAME ${DEP_NAME})

#if(CMAKE_SYSTEM_NAME STREQUAL "Android")
#    set(${DEP_NAME}_NAME ${DEP_NAME}_android)
#    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-android.zip")
#elseif(CMAKE_SYSTEM_NAME STREQUAL "iOS")
#    set(${DEP_NAME}_NAME ${DEP_NAME}_ios)
#    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-ios.zip")
#elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#    set(${DEP_NAME}_NAME ${DEP_NAME}_win)
#    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-win.zip")
#elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#    set(${DEP_NAME}_NAME ${DEP_NAME}_macos)
#    set(${DEP_NAME}_URL "https://github.com/Unity-Technologies/com.unity.webrtc/releases/download/M96/webrtc-mac.zip")
#endif()

#set(${DEP_NAME}_SOURCE_DIR  "${CMAKE_CURRENT_BINARY_DIR}/${${DEP_NAME}_NAME}")
#include(ExternalProject)
#ExternalProject_Add( ${${DEP_NAME}_NAME}
#    URL     	${${DEP_NAME}_URL}
##    SOURCE_DIR 	${${DEP_NAME}_SOURCE_DIR}
#)

#set(WEBRTC_SOURCE_DIR ${${DEP_NAME}_SOURCE_DIR})
