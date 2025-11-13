#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <tchar.h>
#include <chrono>

#include "ui/components/custom_button.h"
#include "ui/components/custom_slider.h"
#include "ui/components/custom_toggle.h"
#include "ui/components/custom_card.h"
#include "ui/navigation/sidebar.h"
#include "ui/navigation/topbar.h"
#include "ui/pages/dashboard.h"
#include "monitoring/monitoring_engine.h"
#include <spdlog/spdlog.h>

using namespace PCOptimizer::UI;

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

int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"PCOptimizer", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"PC Optimizer Premium", WS_POPUP | WS_MAXIMIZE, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
    
    spdlog::set_level(spdlog::level::info);
    spdlog::info("PC Optimizer Premium starting...");
    
    Monitor::MonitoringEngine::Get().Start(1000);
    spdlog::info("Monitoring engine started");
    
    UI::Dashboard::Get().Initialize();
    spdlog::info("Dashboard initialized");

    bool show_demo = false;
    float slider_value = 50.0f;
    bool toggle_value = false;
    NavigationPage current_page = NavigationPage::Home;
    
    auto lastFrameTime = std::chrono::steady_clock::now();

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
        
        auto currentTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
        lastFrameTime = currentTime;

        Sidebar::Render(current_page);
        TopBar::Render("Gaming Mode", Sidebar::GetWidth());
        
        ImGui::SetNextWindowPos(ImVec2(Sidebar::GetWidth(), 60), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - Sidebar::GetWidth(), io.DisplaySize.y - 60), ImGuiCond_Always);
        
        ImGui::Begin("MainContent", nullptr, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus);
        
        if (current_page == NavigationPage::Home) {
            UI::Dashboard::Get().Update(deltaTime);
            UI::Dashboard::Get().Render();
        }
        
        ImGui::End();

        if (show_demo)
        {
            ImGui::SetNextWindowPos(ImVec2(Sidebar::GetWidth() + 20, 80), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
            
            ImGui::Begin("PC Optimizer - Custom UI Demo", &show_demo);
            
            ImGui::Text("Custom Components Demo");
            ImGui::Separator();
            
            ImGui::Spacing();
            ImGui::Text("Buttons:");
            if (CustomButton::Render("Primary Button", ImVec2(200, 40), ButtonStyle::Primary)) {
                ImGui::OpenPopup("Button Clicked");
            }
            ImGui::SameLine();
            if (CustomButton::Render("Success", ImVec2(150, 40), ButtonStyle::Success)) {}
            ImGui::SameLine();
            if (CustomButton::Render("Warning", ImVec2(150, 40), ButtonStyle::Warning)) {}
            ImGui::SameLine();
            if (CustomButton::Render("Error", ImVec2(150, 40), ButtonStyle::Error)) {}
            
            if (ImGui::BeginPopupModal("Button Clicked", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Button was clicked!");
                if (CustomButton::Render("Close", ImVec2(100, 30), ButtonStyle::Secondary)) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Text("Slider:");
            CustomSlider::Render("Value", &slider_value, 0.0f, 100.0f, "%.1f%%");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Text("Toggle:");
            CustomToggle::Render("Enable Feature", &toggle_value);
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            
            ImGui::Text("Cards:");
            
            CustomCard::Begin("CPU Monitor", ImVec2(380, 250));
            ImGui::Text("CPU: Intel Core i9-13900K");
            ImGui::Text("Temperature: 45C");
            ImGui::Text("Usage: 23%%");
            CustomCard::End();
            
            ImGui::SameLine();
            
            CustomCard::Begin("GPU Monitor", ImVec2(380, 250));
            ImGui::Text("GPU: NVIDIA RTX 4090");
            ImGui::Text("Temperature: 55C");
            ImGui::Text("Usage: 67%%");
            ImGui::Text("VRAM: 12.5 GB / 24 GB");
            CustomCard::End();
            
            ImGui::End();
        }

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }
    
    spdlog::info("Shutting down...");
    
    UI::Dashboard::Get().Shutdown();
    Monitor::MonitoringEngine::Get().Stop();
    
    spdlog::info("Dashboard and monitoring stopped");

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
