cmake_minimum_required(VERSION 3.14)
project(Hiking)

set(CMAKE_CXX_STANDARD 17)

# 소스 파일 자동 검색
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# 실행 파일 생성
add_executable(Hiking ${SOURCES})

# 실행 파일 CMAKE_SOURCE_DIR로 이동
add_custom_command(TARGET Hiking POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:Hiking>"
    "${CMAKE_SOURCE_DIR}/Hiking.exe"
)

# 컴파일 정의는 타겟 생성 이후에!
target_compile_definitions(Hiking PRIVATE UNICODE _UNICODE)

# 인클루드 경로
target_include_directories(Hiking PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# DX11 링크
target_link_libraries(Hiking
    d3d11
    dxgi
    d3dcompiler
    dxguid
)

# GUI 앱으로 설정 (콘솔 숨김)
set_target_properties(Hiking PROPERTIES
    WIN32_EXECUTABLE ON
)