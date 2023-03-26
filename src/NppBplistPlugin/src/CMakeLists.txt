function(find_files_by_mask RESULT MASK PATH)
    file(GLOB_RECURSE FILES "${PATH}/*${MASK}")
    set(${RESULT} ${FILES} PARENT_SCOPE)
endfunction()

find_files_by_mask(SOURCE_FILES "*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}")
find_files_by_mask(HEADER_FILES "*.h" "${CMAKE_CURRENT_SOURCE_DIR}")
find_files_by_mask(RC_FILES "rsrc.rc" "${CMAKE_CURRENT_SOURCE_DIR}")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(TARGET_TRIPLET "x64-windows-static")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(TARGET_TRIPLET "x86-windows-static")
endif()

add_library(NppBplistPlugin SHARED ${SOURCE_FILES} ${HEADER_FILES} ${RC_FILES})

set_property(TARGET NppBplistPlugin PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(NppBplistPlugin PUBLIC "${VCPKG_INSTALLED_DIR}/${TARGET_TRIPLET}/include")

find_library(PLISTPP_LIB NAMES plist++-2.0 PATHS "${VCPKG_INSTALLED_DIR}/${TARGET_TRIPLET}/lib" NO_DEFAULT_PATH)
find_library(PLIST_LIB NAMES plist-2.0 PATHS "${VCPKG_INSTALLED_DIR}/${TARGET_TRIPLET}/lib" NO_DEFAULT_PATH)

target_link_libraries(NppBplistPlugin PUBLIC ${PLIST_LIB} ${PLISTPP_LIB})