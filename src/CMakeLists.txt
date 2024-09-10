project(zenixa)

file(GLOB src
  main.cpp
  ../vendor/imgui/*.cpp
)

add_library(${PROJECT_NAME} SHARED 
  ${src}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ../vendor/imgui
)

target_link_libraries(${PROJECT_NAME} 
  android
  log
  dobby
  GLESv2
)
