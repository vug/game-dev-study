set(GAME Snake)
add_executable(${GAME}
  Snake.cpp GameStates.cpp Widgets.cpp main.cpp)

target_link_libraries(${GAME} PRIVATE
  SDL2::SDL2main SDL2::SDL2
  SDL2_ttf::SDL2_ttf
)

target_compile_features(${GAME} PRIVATE cxx_std_20)

if(MSVC)
  add_compile_options(/W4) # /WX if warnings should be treated as errors
  # set cwd to ${GAME}/Debug instead of just ${GAME}/
  set_property(TARGET ${GAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

if (WIN32)
# copy the SDL DLL files to the same folder as the executable
add_custom_command(
    TARGET ${GAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2>
    $<TARGET_FILE_DIR:${GAME}>
    VERBATIM)

add_custom_command(
    TARGET ${GAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2_ttf::SDL2_ttf>
    $<TARGET_FILE_DIR:${GAME}>
    VERBATIM)

# copy assets folder (might become too slow if happens after every build)
add_custom_command(
    TARGET ${GAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets/
    $<TARGET_FILE_DIR:${GAME}>/assets/
    VERBATIM)
endif()