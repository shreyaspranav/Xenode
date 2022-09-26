#include "pch"
#include "Log.h"

#ifdef XEN_LOG_ON

namespace Xen {

	Ref<spdlog::logger> Log::m_EngineLogger;
	Ref<spdlog::logger> Log::m_AppLogger;

	void Log::Init()
	{
		std::vector<spdlog::sink_ptr> sinks;

		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Xenode.log", true));

		m_EngineLogger = std::make_shared<spdlog::logger>("XENODE", begin(sinks), end(sinks));
		m_EngineLogger->set_level(spdlog::level::trace);

		m_AppLogger = std::make_shared<spdlog::logger>("APP", begin(sinks), end(sinks));
		m_AppLogger->set_level(spdlog::level::trace);

		spdlog::register_logger(m_EngineLogger);
		spdlog::register_logger(m_AppLogger);
	}
}

#endif // XEN_LOG_ON