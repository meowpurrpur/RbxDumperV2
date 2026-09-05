set(PROJECT_NAME RbxDumperV2)

set(Header_Files
        classes/helper.h
        classes/offset.h
        classes/roblox.h
        classes/vectors.h
        dump/dump.h
        globals.h
        lib/dissasemble.h
        lib/mem.hpp
        lib/json.hpp
        lib/logger.h
        lib/patternScanning.hpp
        lib/rtti.hpp
)

set(Source_Files
        classes/helper.cpp
        classes/offset.cpp
        classes/roblox.cpp
        dump/animation.cpp
        dump/camera.cpp
        dump/dataModel.cpp
        dump/fflags.cpp
        dump/general.cpp
        dump/interactables.cpp
        dump/lighting.cpp
        dump/mouse.cpp
        dump/parts.cpp
        dump/players.cpp
        dump/scripts.cpp
        dump/staticDump.cpp
        dump/ui.cpp
        dump/workspace.cpp
        lib/rtti.cpp
        main.cpp
)

add_executable(${PROJECT_NAME}
        ${Header_Files}
        ${Source_Files}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        ZYDIS_STATIC_BUILD
        _CRT_SECURE_NO_WARNINGS
        _CONSOLE
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /permissive-
            /sdl
            /W3
    )

    target_link_options(${PROJECT_NAME} PRIVATE
            /DEBUG
            /SUBSYSTEM:CONSOLE
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${PROJECT_NAME} PRIVATE
                /Oi
                /Gy
        )

        target_link_options(${PROJECT_NAME} PRIVATE
                /OPT:REF
                /OPT:ICF
        )
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
        Zydis
)
