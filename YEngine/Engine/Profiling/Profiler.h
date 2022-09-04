#pragma once
#include <Windows.h>
#include <string>
#include <chrono>
#include <Profiling/D3D11Utility.h>

namespace Engine
{
	class Profiler
	{
	public:
		Profiler();

		void FrameTime();
		void Initialize();
		DWORD GetFps()					{ return m_fps; }
		//float GetFrameTime()			{ return time_elapsed / 1000.f; }
		std::string GetGPU()			{ return m_gpu_name; }

		uint32_t GetTotalVRAM()			{ return m_gpu_memory_total; }

		std::string& GetMetrics()		{ return buffer; }

	private:
		DWORD m_frame_cnt = 0;
		DWORD time_elapsed = 0;
		DWORD m_fps = 0;
		DWORD old_time = 0;
		DWORD new_time = 0;
		DWORD delta_time = 0;

		//Frame
		std::chrono::high_resolution_clock::time_point m_time_start;
		double m_delta_time_ms = 0.0f;
	private:
		// Hardware - GPU
		std::string m_gpu_name = "N/A";
		std::string m_gpu_driver = "N/A";
		std::string m_gpu_api = "N/A";

		uint32_t m_gpu_memory_available = 0;
		uint32_t m_gpu_memory_total = 0;
		uint32_t m_gpu_memory_used = 0;

		UINT countTextures;

	private:
		void UpdateMetricsString();
		std::string buffer = "N/A";
	};
}