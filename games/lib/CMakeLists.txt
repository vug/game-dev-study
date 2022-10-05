set(LIB gds)
add_library(${LIB} STATIC
  gds.cpp gds.h
)

target_include_directories(${LIB} PUBLIC .)

target_link_libraries(${LIB} PUBLIC
  SDL2::SDL2main SDL2::SDL2
  SDL2_ttf::SDL2_ttf
)

target_compile_features(${LIB} PRIVATE cxx_std_20)

if(MSVC)
  add_compile_options(/W4) # /WX if warnings should be treated as errors

  # set cwd to ${LIB}/Debug instead of just ${GAME}/
  set_property(TARGET ${LIB} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()