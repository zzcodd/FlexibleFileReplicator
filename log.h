/*
 * @Author: Zhang Xuhui
 * @Date: 2022-12-12 10:35:32
 * @LastEditTime: 2022-12-16 16:39:30
 * @LastEditors: Na Sen
 * @Description: 
 * @FilePath: /ss_tiny/common/config.h
 * Copyright (c) 2018-2022 Changsha Intelligent Driving Institute Ltd. All rights reserved.
 */
#pragma once
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/logger.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/bundled/printf.h>
#include <spdlog/sinks/daily_file_sink.h>

class CLogger {
  public:
    /// let CLogger like stream
    class LogStream : public std::ostringstream {
      public:
        LogStream(const spdlog::source_loc& _loc,
            spdlog::level::level_enum _lvl, std::string _prefix);

        ~LogStream(void);

      private:
        void flush(void);

        spdlog::source_loc loc;
        spdlog::level::level_enum lvl = spdlog::level::info;
        std::string prefix;
    };

    bool Init(std::string log_file_path, std::string prefix);

    static CLogger& Get(void);
 
    void Shutdown(void);

    spdlog::level::level_enum Level(void);

    void SetLevel(spdlog::level::level_enum lvl);

    void SetFlushOn(spdlog::level::level_enum lvl);

    static const char* GetShortName(std::string path);

    template <typename... Args>
      void Log(const spdlog::source_loc& loc, spdlog::level::level_enum lvl,
          const char* fmt, const Args &... args)
      {
        Log(loc, lvl, fmt::sprintf(fmt, args...).c_str());
      }

  private:

    void Log(const spdlog::source_loc& loc, spdlog::level::level_enum lvl,
        const char *data);

    void DebugMonitor(void *p);

    std::vector<spdlog::sink_ptr> sinks;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> daily_trace;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> daily_debug;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> daily_info;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> daily_warn;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> daily_error;
    std::shared_ptr<spdlog::sinks::daily_file_sink_mt> daily_critical;

    CLogger(void) = default;
    ~CLogger(void) = default;

    CLogger(const CLogger&) = delete;
    void operator=(const CLogger&) = delete;

    std::atomic_bool _is_inited = ATOMIC_VAR_INIT(false);
    std::atomic_bool _is_debug_enable = ATOMIC_VAR_INIT(false);
    spdlog::level::level_enum _log_level = spdlog::level::trace;
};

#define __FILENAME__ (CLogger::GetShortName(__FILE__))
 
#define ATRACE	CLogger::LogStream({},spdlog::level::trace, "")
#define ADEBUG	CLogger::LogStream({__FILENAME__, __LINE__, __FUNCTION__},spdlog::level::debug, "")
#define AINFO	  CLogger::LogStream({__FILENAME__, __LINE__, __FUNCTION__},spdlog::level::info, "")
#define AWARN	  CLogger::LogStream({},spdlog::level::warn, "")
#define AERROR	CLogger::LogStream({},spdlog::level::err, "")
#define AFATAL	CLogger::LogStream({},spdlog::level::critical, "")

#define PRINT_TRACE(msg,...)  CLogger::Get().Log({}, spdlog::level::trace, msg, ##__VA_ARGS__)
#define PRINT_DEBUG(msg,...)  CLogger::Get().Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::debug, msg, ##__VA_ARGS__)
#define PRINT_INFO(msg,...)  CLogger::Get().Log({__FILENAME__, __LINE__, __FUNCTION__}, spdlog::level::info, msg, ##__VA_ARGS__)
#define PRINT_WARN(msg,...)  CLogger::Get().Log({}, spdlog::level::warn, msg, ##__VA_ARGS__)
#define PRINT_ERROR(msg,...)  CLogger::Get().Log({}, spdlog::level::err, msg, ##__VA_ARGS__)
#define PRINT_FATAL(msg,...)  CLogger::Get().Log({}, spdlog::level::info, msg, ##__VA_ARGS__)
