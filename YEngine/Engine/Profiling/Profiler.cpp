#include "Profiler.h"
#include <iostream>
#include <dxgi.h>
#include <vector>
#include <Macros/macros.h>
#include <Parser/Drawable/Texture.h>
#pragma comment(lib, "dxgi.lib")

template <typename T>
constexpr void release(T* ptr)
{
    if (ptr)
    {
        ptr->Release();
    }
}

inline void DetectAdapters(std::string& nameGPU, uint32_t& m_gpu_memory_total)
{
    // Create DirectX graphics interface factory
    IDXGIFactory1* factory;
    const auto result = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(result))
    {
        //LOG_ERROR("Failed to create a DirectX graphics interface factory, %s.", dxgi_error_to_string(result));
        return;
    }

    const auto get_available_adapters = [](IDXGIFactory1* factory)
    {
        uint32_t i = 0;
        IDXGIAdapter* adapter;
        std::vector<IDXGIAdapter*> adapters;
        while (factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            adapters.emplace_back(adapter);
            ++i;
        }

        return adapters;
    };

    // Get all available adapters
    auto adapters = get_available_adapters(factory);
    release(factory);
    if (adapters.empty())
    {
        LOG_ERROR("Couldn't find any adapters");
        return;
    }

    // Save all available adapters
    DXGI_ADAPTER_DESC adapter_desc;
    for (auto display_adapter : adapters)
    {
        if (FAILED(display_adapter->GetDesc(&adapter_desc)))
        {
            LOG_ERROR("Failed to get adapter description");
            continue;
        }

        // Of course it wouldn't be simple, lets convert the device name
        auto def_char = ' ';
        char name[128];
        WideCharToMultiByte(CP_ACP, 0, adapter_desc.Description, -1, name, 128, &def_char, nullptr);

        nameGPU = name;

        m_gpu_memory_total = static_cast<uint32_t> ( adapter_desc.DedicatedVideoMemory );

        break;
    }
}

Engine::Profiler::Profiler()
{
    DetectAdapters( m_gpu_name, m_gpu_memory_total );
    countTextures = Bind::Texture::GetCountTextures();
}

void Engine::Profiler::FrameTime()
{
    m_frame_cnt++; // c ������ ������ ������������� �� 1, �.�. ��� ���-�� ������ ���. �� �������� �� ���������� �������
    new_time = GetTickCount(); // ���������� � ������� ����� ����� ���� ����������� ���������
    delta_time = new_time - old_time; // ������ ������� ������� �������� �� ������ �� �����
    time_elapsed += delta_time; // � ���������� ��� ������� � ������������

    if (time_elapsed >= 500) // ���� � ������������ ���������� 0.5 ���, ��
    {
        m_fps = 1000 * m_frame_cnt / time_elapsed; // ����� ���-�� ������ ��������� �� ���������� �� �����������(������ �������� 0.5 ���)
        time_elapsed = 0; // �������� �����������, ��� ���������� ��������
        m_frame_cnt = 0; // � �������� ���-�� ������
    }

    std::chrono::duration<double, std::milli> delta_time = std::chrono::high_resolution_clock::now() - m_time_start;
    m_delta_time_ms = static_cast<double>( delta_time.count() );

    UpdateMetricsString();
}


void Engine::Profiler::Initialize()
{
    old_time = GetTickCount();
    m_time_start = std::chrono::high_resolution_clock::now();
}

void Engine::Profiler::UpdateMetricsString()
{
    static const char* text =
        //Times

        "FPS:\t\t%d\n"
        "Time:\t%.2f ms\n"

        //GPU

        "GPU:\t%s\n"
        "VRAM:\t%d MB\n"
        "Textures:\t%d";
    static char buffer[2048];
    sprintf_s
    (
        buffer, text,

        m_fps, m_delta_time_ms,
        m_gpu_name.c_str(), m_gpu_memory_total / 1024 / 1024, countTextures
    );
    this->buffer = std::string(buffer);
}
