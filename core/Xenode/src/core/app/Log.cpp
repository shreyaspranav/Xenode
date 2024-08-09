#include "pch"
#include "Log.h"

#ifdef XEN_LOG_ON

#ifdef XEN_DEVICE_DESKTOP

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>


#elif XEN_PLATFORM_ANDROID
#include <spdlog/sinks/android_sink.h>
#endif

namespace Xen {

	Ref<spdlog::logger> Log::m_EngineLogger;
	Ref<spdlog::logger> Log::m_AppLogger;

	void Log::Init()
	{
	#ifdef XEN_DEVICE_DESKTOP
		std::vector<spdlog::sink_ptr> sinks;

		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Xenode.log", true));

		m_EngineLogger = std::make_shared<spdlog::logger>("XENODE", begin(sinks), end(sinks));
		m_EngineLogger->set_level(spdlog::level::trace);

		m_AppLogger = std::make_shared<spdlog::logger>("APP", begin(sinks), end(sinks));
		m_AppLogger->set_level(spdlog::level::trace);

		spdlog::register_logger(m_EngineLogger);
		spdlog::register_logger(m_AppLogger);

	#elif XEN_PLATFORM_ANDROID
		m_EngineLogger = spdlog::android_logger_mt("Engine Logger", "XENODE");
		m_EngineLogger->set_level(spdlog::level::trace);

		m_AppLogger = spdlog::android_logger_mt("App Logger", "APP");
		m_AppLogger->set_level(spdlog::level::trace);

	#endif // XEN_DEVICE_DESKTOP
	}
}

#endif // XEN_LOG_ON