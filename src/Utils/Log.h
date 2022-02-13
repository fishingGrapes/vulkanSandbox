#pragma once

#include "spdlog/logger.h"
#include "spdlog/fmt/ostr.h"

class CLog
{
public:
	CLog() = delete;
	static void Initialize();

	inline static std::shared_ptr<spdlog::logger> GetCoreLogger()
	{
		return coreLogger_;
	};

private:
	static std::shared_ptr<spdlog::logger> coreLogger_;
};


#if defined(VKS_DEBUG)

#define VS_TRACE(...)	    CLog::GetCoreLogger()->trace(__VA_ARGS__)
#define VS_INFO(...)       CLog::GetCoreLogger()->info(__VA_ARGS__)
#define VS_WARN(...)       CLog::GetCoreLogger()->warn(__VA_ARGS__)
#define VS_ERROR(...)      CLog::GetCoreLogger()->error(__VA_ARGS__)
#define VS_CRITICAL(...)   CLog::GetCoreLogger()->critical(__VA_ARGS__)


#else

#define VS_TRACE(...)	   
#define VS_INFO(...)       
#define VS_WARN(...)       
#define VS_ERROR(...)      
#define VS_CRITICAL(...)   


#endif
