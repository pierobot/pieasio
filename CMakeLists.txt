CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)

INCLUDE(CheckCXXSymbolExists)
INCLUDE(CheckIncludeFileCXX)

IF(WIN32)
    CHECK_CXX_SYMBOL_EXISTS(CreateIoCompletionPort "Windows.h" HAS_IOCP)
    IF(HAS_IOCP)
        ADD_DEFINITIONS(-DPIE_ASIO_HAS_IOCP)
    ENDIF()
ELSEIF(UNIX)
    SET(SYSTEM_NAME "${CMAKE_SYSTEM_NAME}" CACHE INTERNAL "")

    IF (SYSTEM_NAME STREQUAL "Linux")
#       Use epoll and aio in Linux 
        CHECK_INCLUDE_FILE_CXX("sys/epoll.h" HAS_SYS_EPOLL_HEADER)

        IF (HAS_SYS_EPOLL_HEADER)
            CHECK_CXX_SYMBOL_EXISTS(epoll_create "sys/epoll.h" HAS_EPOLL)
            IF (HAS_EPOLL)
                ADD_DEFINITIONS(-DPIE_ASIO_HAS_EPOLL)
            ENDIF()
        ELSE()
            MESSAGE(ERROR "Unable to locate sys/epoll.h")
        ENDIF()
   ENDIF() 
ENDIF()

IF(WIN32)
ELSEIF(UNIX)
    IF (SYSTEM_NAME STREQUAL "Linux")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -lpthread")
    ENDIF()
ENDIF()

ADD_SUBDIRECTORY(tests)

