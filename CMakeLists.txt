cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME Hiking)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})

# 소스 파일 자동 검색
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# 실행 파일 생성
add_executable(${PROJECT_NAME} ${SOURCES})

include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# 컴파일 정의는 타겟 생성 이후에!
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)

# 인클루드 경로
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(INSTALL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/out/build/x64-Debug/install/include")
else()
    set(INSTALL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/out/build/x64-Release/install/include")
endif()

# PhysX 라이브러리 경로 추가 (예시)
set(PHYSX_INCLUDE_DIR "C:/Program Files/vcpkg/installed/x64-windows/include/physx")
set(PHYSX_INCLUDE_DIR_COOK "C:/Program Files/vcpkg/installed/x64-windows/include/physx/cooking")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PHYSX_LIB_DIR "C:/Program Files/vcpkg/installed/x64-windows/debug/lib")
    set(PHYSX_DLL_DIR "C:/Program Files/vcpkg/installed/x64-windows/debug/bin")
else()
    set(PHYSX_LIB_DIR "C:/Program Files/vcpkg/installed/x64-windows/lib")
    set(PHYSX_DLL_DIR "C:/Program Files/vcpkg/installed/x64-windows/bin")
endif()


# PhysX 헤더 경로 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${PHYSX_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PHYSX_INCLUDE_DIR_COOK})

# PhysX 라이브러리 링크
target_link_directories(${PROJECT_NAME} PUBLIC ${PHYSX_LIB_DIR})

file(GLOB PHYSX_LIBS "${PHYSX_LIB_DIR}/*.lib")

# PhysX 라이브러리 링크 (실제 파일 이름에 맞게 수정)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PHYSX_LIBS})

# 추가 인클루드 경로 설정
target_include_directories(${PROJECT_NAME} PRIVATE ${INSTALL_INCLUDE_DIR})

# DX11 라이브러리 경로 추가
target_link_libraries(${PROJECT_NAME} PUBLIC
    d3d11
    dxgi
    d3dcompiler
    dxguid
)

# GUI 앱으로 설정 (콘솔 숨김)
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
)

# Dependency들이 먼저 build 될 수 있게 관계 설정 / 뒤에서 부터 컴파일
add_dependencies(${PROJECT_NAME} ${DEP_LIST})

# asset 복사
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
)

# DLL 파일을 찾기
file(GLOB PHYSX_DLLS "${PHYSX_DLL_DIR}/*.dll")

# DLL 파일을 빌드 디렉토리로 복사
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PHYSX_DLLS}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
)