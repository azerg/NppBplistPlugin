function(find_files_by_mask RESULT MASK PATH)
    file(GLOB_RECURSE FILES "${PATH}/*${MASK}")
    set(${RESULT} ${FILES} PARENT_SCOPE)
endfunction()

find_files_by_mask(SOURCE_FILES "*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}")
find_files_by_mask(HEADER_FILES "*.h" "${CMAKE_CURRENT_SOURCE_DIR}")
find_files_by_mask(RC_FILES "rsrc.rc" "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(NppBplistPlugin SHARED ${SOURCE_FILES} ${HEADER_FILES} ${RC_FILES})

set_property(TARGET NppBplistPlugin PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_include_directories(NppBplistPlugin PUBLIC "${VCPKG_INSTALLED_DIR}/${TARGET_TRIPLET}/include")

find_package(unofficial-libplist CONFIG REQUIRED)

target_link_libraries(NppBplistPlugin PUBLIC unofficial::libplist::libplist unofficial::libplist::libplist++)