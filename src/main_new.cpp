#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include <d3d11.h>
#include <tchar.h>
#include <chrono>

#undef min
#undef max

#include "monitoring/monitoring_engine.h"
#include "optimizers/profile_manager.h"
#include "optimizers/thread_optimizer.h"
#include "optimizers/timer_optimizer.h"
#include "optimizers/power_optimizer.h"
#include <spdlog/spdlog.h>

static ID3D11Device*            g_pd3dDevice = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext = nullptr;
static IDXGISwapChain*          g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView*  g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Colors {
    ImVec4 accent = ImColor(0, 150, 255);
    ImVec4 background = ImColor(18, 18, 20);
    ImVec4 backgroundLight = ImColor(26, 26, 28);
    ImVec4 border = ImColor(40, 40, 42);
    ImVec4 text = ImColor(220, 220, 220);
    ImVec4 textDim = ImColor(150, 150, 150);
}

int currentTab = 0;

void RenderDashboard() {
    ImGui::BeginChild("Dashboard", ImVec2(0, 0), false);
    
    auto cpuInfo = Monitor::MonitoringEngine::Get().GetCPUInfo();
    auto gpuInfo = Monitor::MonitoringEngine::Get().GetGPUInfo();
    auto ramInfo = Monitor::MonitoringEngine::Get().GetRAMInfo();
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
    ImGui::TextColored(Colors::text, "PC Optimizer Premium - Dashboard");
    ImGui::PopFont();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Columns(2, "stats", false);
    
    ImGui::BeginChild("CPUPanel", ImVec2(0, 200), true);
    ImGui::TextColored(Colors::accent, "CPU Monitor");
    ImGui::Separator();
    
    if (!cpuInfo.empty()) {
        float avgUsage = 0.0f;
        for (const auto& core : cpuInfo) {
            avgUsage += core.usage;
        }
        avgUsage /= cpuInfo.size();
        
        ImGui::Text("Cores: %d", (int)cpuInfo.size());
        ImGui::Text("Average Usage: %.1f%%", avgUsage);
        
        ImGui::Spacing();
        ImGui::Text("Per-Core Usage:");
        for (size_t i = 0; i < cpuInfo.size() && i < 16; i++) {
            ImGui::ProgressBar(cpuInfo[i].usage / 100.0f, ImVec2(-1, 0), "");
            ImGui::SameLine();
            ImGui::Text("Core %d: %.1f%%", (int)i, cpuInfo[i].usage);
        }
    } else {
        ImGui::TextColored(Colors::textDim, "No CPU data available");
    }
    ImGui::EndChild();
    
    ImGui::NextColumn();
    
    ImGui::BeginChild("GPUPanel", ImVec2(0, 200), true);
    ImGui::TextColored(Colors::accent, "GPU Monitor");
    ImGui::Separator();
    ImGui::Text("Name: %s", gpuInfo.name.c_str());
    ImGui::Text("Usage: %.1f%%", gpuInfo.usage);
    ImGui::Text("Temperature: %.1f C", gpuInfo.temperature);
    ImGui::Text("VRAM: %.1f / %.1f GB", gpuInfo.memoryUsage, gpuInfo.memoryTotal);
    ImGui::EndChild();
    
    ImGui::Columns(1);
    
    ImGui::Spacing();
    
    ImGui::BeginChild("RAMPanel", ImVec2(0, 150), true);
    ImGui::TextColored(Colors::accent, "RAM Monitor");
    ImGui::Separator();
    ImGui::Text("Total: %.1f GB", ramInfo.totalGB);
    ImGui::Text("Used: %.1f GB (%.1f%%)", ramInfo.usedGB, ramInfo.usagePercent);
    ImGui::ProgressBar(ramInfo.usagePercent / 100.0f, ImVec2(-1, 24));
    ImGui::EndChild();
    
    ImGui::Spacing();
    
    ImGui::BeginChild("ProfilesPanel", ImVec2(0, 120), true);
    ImGui::TextColored(Colors::accent, "Quick Profiles");
    ImGui::Separator();
    
    auto currentProfile = Optimizer::ProfileManager::Get().GetCurrentProfileType();
    
    if (ImGui::Button("Gaming Mode", ImVec2(150, 40))) {
        Optimizer::ProfileManager::Get().ApplyProfile(Optimizer::ProfileType::Gaming);
        spdlog::info("Applied Gaming Profile");
    }
    if (currentProfile == Optimizer::ProfileType::Gaming) {
        ImGui::SameLine();
        ImGui::TextColored(Colors::accent, "[ACTIVE]");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Streaming Mode", ImVec2(150, 40))) {
        Optimizer::ProfileManager::Get().ApplyProfile(Optimizer::ProfileType::Streaming);
        spdlog::info("Applied Streaming Profile");
    }
    if (currentProfile == Optimizer::ProfileType::Streaming) {
        ImGui::SameLine();
        ImGui::TextColored(Colors::accent, "[ACTIVE]");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Workstation Mode", ImVec2(150, 40))) {
        Optimizer::ProfileManager::Get().ApplyProfile(Optimizer::ProfileType::Workstation);
        spdlog::info("Applied Workstation Profile");
    }
    if (currentProfile == Optimizer::ProfileType::Workstation) {
        ImGui::SameLine();
        ImGui::TextColored(Colors::accent, "[ACTIVE]");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Balanced Mode", ImVec2(150, 40))) {
        Optimizer::ProfileManager::Get().ApplyProfile(Optimizer::ProfileType::Balanced);
        spdlog::info("Applied Balanced Profile");
    }
    if (currentProfile == Optimizer::ProfileType::Balanced) {
        ImGui::SameLine();
        ImGui::TextColored(Colors::accent, "[ACTIVE]");
    }
    ImGui::EndChild();
    
    ImGui::EndChild();
}

void RenderPerformance() {
    ImGui::BeginChild("Performance", ImVec2(0, 0), false);
    ImGui::TextColored(Colors::accent, "Performance Optimization");
    ImGui::Separator();
    ImGui::Spacing();
    
    static float timerResolution = 1.0f;
    static int selectedPowerPlanIdx = 0;
    static std::vector<Optimizer::PowerPlan> cachedPowerPlans;
    static bool powerPlansLoaded = false;
    
    if (ImGui::CollapsingHeader("Timer Resolution", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        
        auto& timerOpt = Optimizer::TimerOptimizer::Get();
        auto caps = timerOpt.GetCapabilities();
        
        ImGui::Text("System Capabilities:");
        ImGui::Text("  Minimum: %.2f ms", caps.minResolution);
        ImGui::Text("  Maximum: %.2f ms", caps.maxResolution);
        ImGui::Text("  Current: %.2f ms", caps.currentResolution);
        
        ImGui::Spacing();
        
        ImGui::SliderFloat("Target Resolution (ms)", &timerResolution, 0.5f, 15.6f, "%.2f ms");
        
        if (ImGui::Button("Apply Timer Resolution", ImVec2(200, 30))) {
            timerOpt.SetTimerResolution(timerResolution);
            spdlog::info("Set timer resolution to {:.2f}ms", timerResolution);
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Reset to Default", ImVec2(200, 30))) {
            timerOpt.ResetTimerResolution();
            spdlog::info("Reset timer resolution to default");
        }
        
        ImGui::Unindent();
        ImGui::Spacing();
    }
    
    if (ImGui::CollapsingHeader("Power Plan Management", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Indent();
        
        if (ImGui::Button("Refresh Power Plans", ImVec2(200, 30)) || !powerPlansLoaded) {
            cachedPowerPlans = Optimizer::PowerOptimizer::Get().GetPowerPlans();
            powerPlansLoaded = true;
            
            for (size_t i = 0; i < cachedPowerPlans.size(); i++) {
                if (cachedPowerPlans[i].active) {
                    selectedPowerPlanIdx = (int)i;
                    break;
                }
            }
        }
        
        ImGui::Spacing();
        
        if (powerPlansLoaded && !cachedPowerPlans.empty()) {
            ImGui::Text("Available Power Plans:");
            
            for (size_t i = 0; i < cachedPowerPlans.size(); i++) {
                bool selected = (selectedPowerPlanIdx == (int)i);
                std::string label = cachedPowerPlans[i].name;
                if (cachedPowerPlans[i].active) {
                    label += " [ACTIVE]";
                }
                
                if (ImGui::Selectable(label.c_str(), &selected)) {
                    selectedPowerPlanIdx = (int)i;
                }
            }
            
            ImGui::Spacing();
            
            if (ImGui::Button("Apply Selected Plan", ImVec2(200, 30))) {
                if (selectedPowerPlanIdx >= 0 && selectedPowerPlanIdx < cachedPowerPlans.size()) {
                    Optimizer::PowerOptimizer::Get().SetActivePlan(cachedPowerPlans[selectedPowerPlanIdx].guid);
                    cachedPowerPlans = Optimizer::PowerOptimizer::Get().GetPowerPlans();
                    spdlog::info("Applied power plan: {}", cachedPowerPlans[selectedPowerPlanIdx].name);
                }
            }
        } else {
            ImGui::TextColored(Colors::textDim, "Click 'Refresh Power Plans' to load");
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Advanced Power Settings:");
        
        if (ImGui::Button("Disable Core Parking", ImVec2(200, 30))) {
            Optimizer::PowerOptimizer::Get().SetCoreParking(100, 100);
            spdlog::info("Disabled core parking (100%% unparked)");
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Enable Core Parking", ImVec2(200, 30))) {
            Optimizer::PowerOptimizer::Get().SetCoreParking(50, 100);
            spdlog::info("Enabled core parking (50%% min)");
        }
        
        if (ImGui::Button("Disable CPU Throttling", ImVec2(200, 30))) {
            Optimizer::PowerOptimizer::Get().DisableThrottling();
            spdlog::info("Disabled CPU throttling");
        }
        
        ImGui::Unindent();
        ImGui::Spacing();
    }
    
    if (ImGui::CollapsingHeader("Thread & Process Optimization")) {
        ImGui::Indent();
        
        ImGui::TextColored(Colors::textDim, "Process affinity and priority controls");
        ImGui::TextColored(Colors::textDim, "Use Process Monitor tab to select and optimize specific processes");
        
        ImGui::Unindent();
        ImGui::Spacing();
    }
    
    ImGui::EndChild();
}

void RenderSettings() {
    ImGui::BeginChild("Settings", ImVec2(0, 0), false);
    ImGui::TextColored(Colors::accent, "Settings");
    ImGui::Separator();
    ImGui::Spacing();
    
    static bool startWithWindows = false;
    static int pollingRate = 1000;
    
    ImGui::Checkbox("Start with Windows", &startWithWindows);
    ImGui::SliderInt("Monitoring Refresh Rate (ms)", &pollingRate, 100, 5000);
    
    if (ImGui::Button("Save Settings", ImVec2(120, 30))) {
        spdlog::info("Settings saved");
    }
    
    ImGui::EndChild();
}

int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"PCOptimizer", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"PC Optimizer Premium", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
    
    spdlog::set_level(spdlog::level::info);
    spdlog::info("PC Optimizer Premium starting...");
    
    Monitor::MonitoringEngine::Get().Start(1000);
    spdlog::info("Monitoring engine started");

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        
        ImGui::Begin("PC Optimizer", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        ImGui::BeginChild("Sidebar", ImVec2(200, 0), true);
        ImGui::TextColored(Colors::accent, "PC OPTIMIZER");
        ImGui::Separator();
        ImGui::Spacing();
        
        if (ImGui::Selectable("Dashboard", currentTab == 0, 0, ImVec2(0, 30))) currentTab = 0;
        if (ImGui::Selectable("Performance", currentTab == 1, 0, ImVec2(0, 30))) currentTab = 1;
        if (ImGui::Selectable("Graphics", currentTab == 2, 0, ImVec2(0, 30))) currentTab = 2;
        if (ImGui::Selectable("Network", currentTab == 3, 0, ImVec2(0, 30))) currentTab = 3;
        if (ImGui::Selectable("Settings", currentTab == 4, 0, ImVec2(0, 30))) currentTab = 4;
        
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        ImGui::BeginChild("Content", ImVec2(0, 0), false);
        
        switch (currentTab) {
            case 0: RenderDashboard(); break;
            case 1: RenderPerformance(); break;
            case 4: RenderSettings(); break;
            default:
                ImGui::TextColored(Colors::textDim, "Coming soon...");
                break;
        }
        
        ImGui::EndChild();
        
        ImGui::End();

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }
    
    spdlog::info("Shutting down...");
    Monitor::MonitoringEngine::Get().Stop();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED)
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
