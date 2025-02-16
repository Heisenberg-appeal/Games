cmake_minimum_required(VERSION 3.6)
project(nativerender)

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../..)
set(CLASSES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Classes)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fexceptions -fsigned-char")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile)
    set(res_raw_folders "${CMAKE_CURRENT_SOURCE_DIR}/../resources/rawfile")
    file(GLOB ALL_RESOURCES_FILES ${CLASSES_PATH}/../../res ${COCOS2DX_ROOT_PATH}/tests/cpp-tests/Resources/*)
    file(COPY ${ALL_RESOURCES_FILES} DESTINATION ${res_raw_folders})

    file(GLOB ALL_SCRIPT_FILES "${COCOS2DX_ROOT_PATH}/cocos/scripting/lua-bindings/script/*")
    file(COPY ${ALL_SCRIPT_FILES} DESTINATION ${res_raw_folders}/cocos)

    file(GLOB ALL_SOURCE_FILES ${CLASSES_PATH}/../../src)
    file(COPY ${ALL_SOURCE_FILES} DESTINATION ${res_raw_folders})
endif()

add_library(${PROJECT_NAME} SHARED  main.cpp
                                    napi_init.cpp)

include_directories(
                    ${COCOS2DX_ROOT_PATH}/external/lua/luajit/include
                    )
target_include_directories(${PROJECT_NAME} PUBLIC ${CLASSES_PATH}
                                           PUBLIC ${COCOS2DX_ROOT_PATH}/cocos
                                           PUBLIC ${COCOS2DX_ROOT_PATH}/cocos/platform/ohos)

add_subdirectory(${COCOS2DX_ROOT_PATH}/tests/lua-tests/project lua_test)
find_library(hilog-lib hilog_ndk.z)
find_library(libuv-lib uv)
target_link_libraries(${PROJECT_NAME} PUBLIC lua_test ext_luajit ${hilog-lib} ${libuv-lib})