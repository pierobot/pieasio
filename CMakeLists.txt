CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)

project(pie_asio CXX)

INCLUDE(CheckCXXSymbolExists)
INCLUDE(CheckIncludeFileCXX)

IF(WIN32)
    CHECK_CXX_SYMBOL_EXISTS(CreateIoCompletionPort "Windows.h" HAS_IOCP)
    IF(HAS_IOCP)
        ADD_DEFINITIONS(-DPIE_ASIO_HAS_IOCP)
    ENDIF()
ELSEIF(UNIX)
    SET(SYSTEM_NAME "${SYSTEM_NAME}" CACHE INTERNAL "")

    IF (SYSTEM_NAME STREQUAL "Linux")
#       Use epoll and aio in Linux  
        CHECK_INCLUDE_FILE_CXX("sys/epoll.h" HAS_SYS_EPOLL_HEADER)
        CHECK_INCLUDE_FILE_CXX("aio.h" HAS_AIO_HEADER)

        IF (HAS_SYS_EPOLL_HEADER)
            CHECK_CXX_SYMBOL_EXISTS(epoll_create "sys/epoll.h" HAS_EPOLL)
            IF (HAS_EPOLL)
                ADD_DEFINITIONS(-DPIE_ASIO_HAS_EPOLL)
            ENDIF()
        ELSE()
            MESSAGE(ERROR "Unable to locate sys/epoll.h")
        ENDIF()

        IF(HAS_AIO_HEADER)
            CHECK_CXX_SYMBOL_EXISTS(aio_init "aio.h" HAS_AIO)
            IF(HAS_AIO)
                ADD_DEFINITIONS(-DPIE_ASIO_HAS_AIO)
            ENDIF()
        ELSE()
            MESSAGE(ERROR "Unable to locate aio.h")
        ENDIF()
    ENDIF() 
ENDIF()

ADD_SUBDIRECTORY(tests)
