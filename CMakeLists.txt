cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0074 NEW)

if (BEAM_SIGN_PACKAGE AND WIN32)
  #  add_custom_target(signing ALL DEPENDS beam-wallet-ui COMMENT "Signing Package...")

    find_program(SIGNTOOL_EXECUTABLE signtool.exe)
    message("SignTool: ${SIGNTOOL_EXECUTABLE}" )

    set(WINDOWS_IDENTITIY_PFX_FILE "THE_PFX_FILE_IS_MISSING_HERE!" CACHE STRING "PFX file, which represents the identity of the developer.")
    set(WINDOWS_IDENTITIY_PFX_PASSWORD "" CACHE STRING "Password, which unlocks the PFX file (optional)")
    set(BEAM_SIGN_SUBJECT_NAME "" CACHE STRING "Beam subjet")
    set(SIGNTOOL_OPTIONS /tr http://timestamp.digicert.com /td sha256 /fd sha256 /n "${BEAM_SIGN_SUBJECT_NAME}")
    
    #set(SIGNTOOL_OPTIONS /fd SHA256 /a /f ${WINDOWS_IDENTITIY_PFX_FILE} /tr http://timestamp.digicert.com  )
    
    #if(NOT ${WINDOWS_IDENTITIY_PFX_PASSWORD} EQUAL "")
    #    set(SIGNTOOL_OPTIONS ${SIGNTOOL_OPTIONS} /p ${WINDOWS_IDENTITIY_PFX_PASSWORD})
    #else()
    #    message(FATAL_ERROR "You can't sign your executables without passing your password here!")
    #endif()

    #if(EXISTS ${WINDOWS_IDENTITIY_PFX_FILE})
    #    message(STATUS "Signing executables with: " ${WINDOWS_IDENTITIY_PFX_FILE})
    #    if(${WINDOWS_IDENTITIY_PFX_PASSWORD})
    #        message(WARNING "USE WITH CAUTION: Password for the PFX file has been set!")
    #    endif()
    #else()
    #    message(WARNING "Could not find the PFX file. Can not sign the executables!")
    #endif()
endif()

if(BEAM_LINK_TYPE MATCHES "Static")
    set(BEAM_USE_STATIC TRUE)
endif()

option(BEAM_USE_STATIC_QT "Build with staticaly linked QT library" FALSE)
if (BEAM_USE_STATIC AND NOT BEAM_USE_STATIC_QT)
    set(BEAM_USE_STATIC_RUNTIME FALSE)
endif()

# should be before project and git 
set(BEAM_WALLET_CLIENT_LIBRARY TRUE)
set(BEAM_TESTS_ENABLED FALSE)
add_subdirectory(beam)

find_package(Git)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        ERROR_VARIABLE RESULT_STRING
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(LENGTH "${RESULT_STRING}" LENGTH_RESULT_STRING)

    if(${LENGTH_RESULT_STRING} EQUAL 0)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE VERSION_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        set(APPVEYOR_REPO_BRANCH_EXISTS $ENV{APPVEYOR_REPO_BRANCH})
        set(TRAVIS_BRANCH_EXISTS $ENV{TRAVIS_BRANCH})

        if(NOT DEFINED BRANCH_NAME)
            execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE BRANCH_NAME
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()

    endif()
else()
    set(BRANCH_NAME "")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/beam)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

if(BEAM_USE_STATIC)
    add_definitions(-DBEAM_USE_STATIC)
endif()

if(BEAM_USE_STATIC_QT)
    add_definitions(-DBEAM_USE_STATIC_QT)
endif()

if (NOT DEFINED BEAM_DEFAULT_NETWORK)
    set(BEAM_DEFAULT_NETWORK "mainnet")
endif()

add_definitions(-DBEAM_DEFAULT_NETWORK=${BEAM_DEFAULT_NETWORK})

if(BEAM_DEFAULT_NETWORK STREQUAL "masternet")
    set(BEAM_SUFFIX_LOWER_CASE "masternet" CACHE INTERNAL "")
    set(BEAM_SUFFIX_UPPER_CASE "Masternet" CACHE INTERNAL "")
    set(BEAM_TARGET_SUFFIX "-${BEAM_SUFFIX_LOWER_CASE}" CACHE INTERNAL "")
    set(BEAM_DISPLAY_SUFFIX " ${BEAM_SUFFIX_UPPER_CASE}" CACHE INTERNAL "")
elseif(BEAM_DEFAULT_NETWORK STREQUAL "testnet")
    set(BEAM_SUFFIX_LOWER_CASE "testnet" CACHE INTERNAL "")
    set(BEAM_SUFFIX_UPPER_CASE "Testnet" CACHE INTERNAL "")
    set(BEAM_TARGET_SUFFIX "-${BEAM_SUFFIX_LOWER_CASE}" CACHE INTERNAL "")
    set(BEAM_DISPLAY_SUFFIX " ${BEAM_SUFFIX_UPPER_CASE}" CACHE INTERNAL "")
elseif(BEAM_DEFAULT_NETWORK STREQUAL "dappnet")
    set(BEAM_SUFFIX_LOWER_CASE "dappnet" CACHE INTERNAL "")
    set(BEAM_SUFFIX_UPPER_CASE "Dappnet" CACHE INTERNAL "")
    set(BEAM_TARGET_SUFFIX "-${BEAM_SUFFIX_LOWER_CASE}" CACHE INTERNAL "")
    set(BEAM_DISPLAY_SUFFIX " ${BEAM_SUFFIX_UPPER_CASE}" CACHE INTERNAL "")
elseif(BEAM_DEFAULT_NETWORK STREQUAL "mainnet")
    set(BEAM_SUFFIX_LOWER_CASE "" CACHE INTERNAL "")
    set(BEAM_SUFFIX_UPPER_CASE "" CACHE INTERNAL "")
    set(BEAM_TARGET_SUFFIX "" CACHE INTERNAL "")
    set(BEAM_DISPLAY_SUFFIX "" CACHE INTERNAL "")
endif()

message("BRANCH_NAME: ${BRANCH_NAME}")

project(beam-ui VERSION ${BEAM_VERSION}.${VERSION_REVISION})
set(VERSION_MAJOR ${BEAM_VERSION_MAJOR})
set(VERSION_MINOR ${BEAM_VERSION_MINOR})

message("BEAM_UI_VERSION: ${PROJECT_VERSION}")
message("BEAM_TARGET_SUFFIX: ${BEAM_TARGET_SUFFIX}")


file(WRITE ${PROJECT_SOURCE_DIR}/beam_version.gen "${PROJECT_VERSION}")

if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(LINUX TRUE)
endif()

if (BEAM_CPP_20_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
else()
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100") # unreferenced formal parameter
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127") # viewmodel/applications/public.cpp
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251") # viewmodel/applications/public.cpp
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4324") # '__blake2b_state': structure was padded due to alignment specifier
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd5240") # qcolor.h(233): 'nodiscard': attribute is ignored in this syntactic position
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2")

    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /INCREMENTAL:NO")
    SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /INCREMENTAL:NO")
    SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /OPT:REF /INCREMENTAL:NO")

    #target_compile_definitions(beam INTERFACE -D_WIN32_WINNT=0x0600)
    #add_definitions(-D_WIN32_WINNT=0x0600)

    if(BEAM_USE_STATIC_RUNTIME)
       # target_compile_options(beam INTERFACE $<IF:$<CONFIG:Debug>,/MTd, /MT>)
       # target_link_options(beam INTERFACE $<IF:$<NOT:$<CONFIG:Debug>>, /OPT:REF /INCREMENTAL:NO, >)
        # use static runtime
        foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
            flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)

            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    endif()
else()
    if(NOT ANDROID AND NOT IOS)
        if(APPLE)
            execute_process(COMMAND uname -m OUTPUT_VARIABLE arch OUTPUT_STRIP_TRAILING_WHITESPACE)
            if ("${arch}" STREQUAL "arm64")
                set(APPLE_M1 TRUE)
            endif()
        endif()

        if(APPLE_M1)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=apple-m1")
        else()
            if(ARCH_NATIVE)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
            else()
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=core2")
            endif()
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pthread -fvisibility=hidden")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

    # disable some warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual") # appears alot in our macro expansions
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-const-variable") # so what?
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function") # mostly in 3rd-party libs
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-value") # proto.h
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nonnull") # yas
    if(APPLE AND
       CMAKE_CXX_COMPILER_ID MATCHES "AppleClang|Clang" AND
       (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 15 OR CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 15))
       add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION) # boost@1.76/1.76.0_4/include/boost/container_hash/hash.hpp:131:33
    endif()
endif()

set(SKIP_INSTALL_ALL ON)
add_subdirectory(3rdparty/zlib EXCLUDE_FROM_ALL)
add_subdirectory(3rdparty/qrcode)
add_subdirectory(3rdparty/quazip)
add_subdirectory(3rdparty/qhttpengine EXCLUDE_FROM_ALL)
add_subdirectory(ui)


########################################################
### PACKAGING ##########################################
########################################################

if (WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_SOURCE_GENERATOR "ZIP")
    set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/ui/icon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/ui/icon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "${PROJECT_SOURCE_DIR}/ui/icon.ico")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "CreateShortCut \\\"$DESKTOP\\\\Beam Wallet${BEAM_DISPLAY_SUFFIX}.lnk\\\" \\\"$INSTDIR\\\\Beam Wallet${BEAM_DISPLAY_SUFFIX}.exe\\\"")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Beam Wallet${BEAM_DISPLAY_SUFFIX}.lnk\\\" \\\"$INSTDIR\\\\Beam Wallet${BEAM_DISPLAY_SUFFIX}.exe\\\"")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "Delete \\\"$DESKTOP\\\\Beam Wallet${BEAM_DISPLAY_SUFFIX}.lnk\\\"")
    set(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Beam Wallet${BEAM_DISPLAY_SUFFIX}.lnk\\\"")

    set(CPACK_NSIS_EXECUTABLES_DIRECTORY .)
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "Beam Wallet${BEAM_DISPLAY_SUFFIX}.exe") 
    set(CPACK_NSIS_PACKAGE_NAME "Beam Wallet${BEAM_DISPLAY_SUFFIX}")

elseif (APPLE)
    set(CPACK_GENERATOR "DragNDrop")
elseif (LINUX)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_NAME "beamwallet${BEAM_SUFFIX_LOWER_CASE}")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Beam <mail@beam-mw.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Network")

endif ()

set(CPACK_PACKAGE_NAME "BeamWallet${BEAM_SUFFIX_UPPER_CASE}")
set(CPACK_PACKAGE_VENDOR "Beam Development Limited")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scalable confidential cryptocurrency")
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_REVISION}")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Beam Wallet${BEAM_DISPLAY_SUFFIX}")

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/TERMS.txt")

include(CPack)

#if (BEAM_SIGN_PACKAGE AND WIN32)
#    add_custom_target(signing ALL  COMMENT "Signing Package...")
#
# 
#     # Signing the installer
#     add_custom_target(signing-installer)
#     set(BEAM_INSTALLER_NAME ${CPACK_NSIS_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}.exe)
#     
#     add_custom_command(
#         TARGET signing-installer
#         COMMAND ${SIGNTOOL_EXECUTABLE} sign ${SIGNTOOL_OPTIONS} ${BEAM_INSTALLER_NAME}
#         ## Other optional options:
#         # /tr timestampServerUrl 
#        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#     )
#endif()
