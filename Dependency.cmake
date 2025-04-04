# ExternalProject 관련 명령어 셋 추가
include(ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

# DirectXTex
ExternalProject_Add(
    dep_DirectXTex
    GIT_REPOSITORY "https://github.com/microsoft/DirectXTex.git"
    GIT_TAG "sept2023"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
    TEST_COMMAND ""
)
set(DEP_LIST ${DEP_LIST} dep_DirectXTex)
set(DEP_LIBS ${DEP_LIBS} DirectXTex)

# assimp
ExternalProject_Add(
	dep_assimp
	GIT_REPOSITORY "https://github.com/assimp/assimp"
	GIT_TAG "v5.0.1"
	GIT_SHALLOW 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
		-DBUILD_SHARED_LIBS=OFF
		-DASSIMP_BUILD_ASSIMP_TOOLS=OFF
		-DASSIMP_BUILD_TESTS=OFF
		-DASSIMP_INJECT_DEBUG_POSTFIX=OFF
		-DASSIMP_BUILD_ZLIB=ON
	TEST_COMMAND ""
)
set(DEP_LIST ${DEP_LIST} dep_assimp)
set(DEP_LIBS ${DEP_LIBS}
	assimp-vc142-mt$<$<CONFIG:Debug>:d>
	zlibstatic$<$<CONFIG:Debug>:d>
	IrrXML$<$<CONFIG:Debug>:d>
	)