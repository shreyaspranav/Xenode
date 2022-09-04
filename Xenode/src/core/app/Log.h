#pragma once

#ifdef XEN_LOG_ON

#include <Core.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Xen {
	class XEN_API Log
	{
	private:
		static Ref<spdlog::logger> m_EngineLogger;
		static Ref<spdlog::logger> m_AppLogger;

	public:
		static void Init();

		static Ref<spdlog::logger> GetEngineLogger() { return m_EngineLogger; }
		static Ref<spdlog::logger> GetAppLogger() { return m_AppLogger; }
	};									 
}

#define XEN_INIT_LOGGER Xen::Log::Init();

#define XEN_ENGINE_LOG_ERROR_SEVERE(...)	Xen::Log::GetEngineLogger()->critical(__VA_ARGS__);
#define XEN_ENGINE_LOG_ERROR(...)			Xen::Log::GetEngineLogger()->error(__VA_ARGS__);
#define XEN_ENGINE_LOG_WARN(...)			Xen::Log::GetEngineLogger()->warn(__VA_ARGS__);	
#define XEN_ENGINE_LOG_INFO(...)			Xen::Log::GetEngineLogger()->info(__VA_ARGS__);	
#define XEN_ENGINE_LOG_TRACE(...)			Xen::Log::GetEngineLogger()->trace(__VA_ARGS__);
																							
#define XEN_APP_LOG_ERROR_SEVERE(...)		Xen::Log::GetAppLogger()->critical(__VA_ARGS__);
#define XEN_APP_LOG_ERROR(...)				Xen::Log::GetAppLogger()->error(__VA_ARGS__);	
#define XEN_APP_LOG_WARN(...)				Xen::Log::GetAppLogger()->warn(__VA_ARGS__);	
#define XEN_APP_LOG_INFO(...)				Xen::Log::GetAppLogger()->info(__VA_ARGS__);	
#define XEN_APP_LOG_TRACE(...)				Xen::Log::GetAppLogger()->trace(__VA_ARGS__);	

#elif XEN_LOG_OFF

#define XEN_INIT_LOGGER 

#define XEN_ENGINE_LOG_ERROR_SEVERE(...)	
#define XEN_ENGINE_LOG_ERROR(...)			
#define XEN_ENGINE_LOG_WARN(...)			
#define XEN_ENGINE_LOG_INFO(...)			
#define XEN_ENGINE_LOG_TRACE(...)			
																							
#define XEN_APP_LOG_ERROR_SEVERE(...)		
#define XEN_APP_LOG_ERROR(...)				
#define XEN_APP_LOG_WARN(...)					
#define XEN_APP_LOG_INFO(...)					
#define XEN_APP_LOG_TRACE(...)		

#endif // XEN_LOG_ON		


