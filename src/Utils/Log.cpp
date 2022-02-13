#include "vkpch.h"
#include "Log.h"

#include "spdlog/spdlog.h"
#include"spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> CLog::coreLogger_;

void CLog::Initialize()
{
	spdlog::set_pattern( "%^[%T] %n: %v%$" );

	coreLogger_ = spdlog::stdout_color_mt( "VULKAN SANDBOX" );
	coreLogger_->set_level( spdlog::level::trace );

	VS_INFO( "Logger Initialized." );
}