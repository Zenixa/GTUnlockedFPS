project(zenixa)
set(ANDROID_ABI armeabi-v7a;arm64-v8a)

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
