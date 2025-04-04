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
    set(INSTALL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/build/x64-Debug/install/include")
else()
    set(INSTALL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/build/x64-Release/install/include")
endif()

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