#include <iostream>
#include <thread>
#include <unistd.h>
#include <dlfcn.h>
#include <dobby.h>
#include <GLES2/gl2.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include "log.h"

// this works on all gt versions with debug symbols in
// ~ Zenixa

constexpr float DESIRED_FPS = 144.f;
static float ScreenSizeX, ScreenSizeY;

void (*SetFPSLimit)(void*, float);
void (*BaseAppDraw)(void*);

void hkBaseAppDraw(void* baseapp)
{
  static bool once = false;

  if (!once) 
  {
    SetFPSLimit(baseapp, DESIRED_FPS);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    io.DisplaySize = ImVec2(ScreenSizeX, ScreenSizeY);
    LOGI("screen size is %d, %d", (int)ScreenSizeX, (int)ScreenSizeY);
    ImGui_ImplOpenGL3_Init("#version 100"); 
    
    once = true;
  }
   
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();

  ImGuiIO& io = ImGui::GetIO();

  float fps = io.Framerate;

  char DisplayString[30];
  snprintf(DisplayString, sizeof(DisplayString), "github.com/Zenixa | FPS: %.2f", fps);
        
  auto draw = ImGui::GetBackgroundDrawList();
	auto watermark = DisplayString;
  auto WatermarkSize = ImGui::CalcTextSize(watermark);

  auto yOffset = 5;
  auto offset = 4;

  draw->AddRectFilled(ImVec2(6, 4 + yOffset), ImVec2((4 * 2) + WatermarkSize.x + 8, 6 + yOffset), ImColor(0.8f, 0.4f, 1.0f, 1.0f));
  draw->AddRectFilled(ImVec2(6, 6 + yOffset), ImVec2((4 * 2) + WatermarkSize.x + 8, 30 + yOffset), ImColor(0.2117647081613541f, 0.2235294133424759f, 0.2470588237047195f, 1.0f));	
	draw->AddText(ImVec2(10, 10 + yOffset), ImColor(255, 255, 255, 255), DisplayString);

  ImGui::Render();
        
  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  
  BaseAppDraw(baseapp);
}

void mthread() 
{
	do 
  {
		sleep(1);
	} while (!dlopen("libgrowtopia.so", RTLD_NOLOAD));

  SetFPSLimit = reinterpret_cast<void(__fastcall*)(void*, float)>(DobbySymbolResolver(nullptr, "_ZN7BaseApp11SetFPSLimitEf"));
  ScreenSizeX = reinterpret_cast<float(__fastcall*)()>(DobbySymbolResolver(nullptr, "_Z15GetScreenSizeXfv"))();
  ScreenSizeY = reinterpret_cast<float(__fastcall*)()>(DobbySymbolResolver(nullptr, "_Z15GetScreenSizeYfv"))();

  auto GetApp = reinterpret_cast<void*(__fastcall*)()>(DobbySymbolResolver(nullptr, "_Z6GetAppv"));
  auto GetAppVersionString = reinterpret_cast<std::string(__fastcall*)(void*)>(DobbySymbolResolver(nullptr, "_ZN3App16GetVersionStringEv"));
  LOGI("app version is this thing %s", GetAppVersionString(GetApp()).c_str());

  DobbyHook(DobbySymbolResolver(nullptr, "_ZN7BaseApp4DrawEv"), (dobby_dummy_func_t)hkBaseAppDraw, (dobby_dummy_func_t*)&BaseAppDraw);
  // holy moly !!!!!!!!!!!1!1!11!
}

__attribute((constructor)) void dllLoad() 
{
  LOGI("libzenixa.so loaded, built on " __DATE__ " " __TIME__);
  std::thread(mthread).detach();
}
