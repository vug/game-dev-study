add_executable(Game1
  main.cpp)

target_link_libraries(
  Game1 PRIVATE
  ${SDL2_LIBRARIES}
)

target_compile_features(Game1 PRIVATE cxx_std_20)

if(MSVC)
  add_compile_options(/W4) # /WX if warnings should be treated as errors
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

if (WIN32)
# copy the .dll file to the same folder as the executable
add_custom_command(
    TARGET Game1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2::SDL2>
    $<TARGET_FILE_DIR:Game1>
    VERBATIM)
endif()