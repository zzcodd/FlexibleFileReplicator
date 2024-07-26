/*
 * @Author: Zhang Xuhui
 * @Date: 2022-12-12 10:35:32
 * @LastEditTime: 2022-12-16 16:39:30
 * @LastEditors: Na Sen
 * @Description: 
 * @FilePath: /ss_tiny/common/config.h
 * Copyright (c) 2018-2022 Changsha Intelligent Driving Institute Ltd. All rights reserved.
 */
#include "log.h"
#ifdef USE_RELEASE_MODE
#include "common/thread/thread_pool.h"
#endif

CLogger::LogStream::LogStream(const spdlog::source_loc& _loc,
    spdlog::level::level_enum _lvl, std::string _prefix)
  : loc(_loc), lvl(_lvl) , prefix(_prefix)
{
}

CLogger::LogStream::~LogStream(void)
{
  flush();
}

void CLogger::LogStream::flush(void)
{
  CLogger::Get().Log(loc, lvl, (prefix + str()).c_str());
}

bool CLogger::Init(std::string log_file_path, std::string prefix)
{
  if (_is_inited) return true;

  try {
    // initialize spdlog
    constexpr std::size_t log_buffer_size = 1 * 1024 * 1024; // 1MB
    // constexpr std::size_t max_file_size = 50 * 1024 * 1024; // 50mb
    //int use_thread_num = std::thread::hardware_concurrency();
    int use_thread_num = 2;
    spdlog::init_thread_pool(log_buffer_size, use_thread_num);

    if (prefix.empty())
      prefix = "log";
    std::string full_prefix = log_file_path + "/" + prefix;
    // trace
    daily_trace = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        full_prefix + "_trace.log", 0, 2);
    sinks.push_back(daily_trace);
    // debug
    daily_debug = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        full_prefix + "_debug.log", 0, 2);
    sinks.push_back(daily_debug);
    // info
    daily_info = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        full_prefix + "_info.log", 0, 2);
    sinks.push_back(daily_info);
    // warn
    daily_warn = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        full_prefix + "_warn.log", 0, 2);
    sinks.push_back(daily_warn);
    // error
    daily_error = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        full_prefix + "_error.log", 0, 2);
    sinks.push_back(daily_error);
    // critical
    daily_critical = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
        full_prefix + "_critical.log", 0, 2);
    sinks.push_back(daily_critical);

    for (spdlog::sink_ptr sink: sinks)
      spdlog::default_logger()->sinks().push_back(sink);

    spdlog::set_pattern("[%L %D %T.%e %P %t %s %#] %v");
    SetFlushOn(spdlog::level::debug);
    //        set_level(_log_level);
    spdlog::flush_every(std::chrono::milliseconds(1));
  }
  catch (const std::exception& e) {
    printf("%s exception: %s\n", __FILE__, e.what());
    return false;
  }

#ifdef USE_RELEASE_MODE
  ThreadPool::Instance()->RegisterThread(
      (thread_pool_callback)&CLogger::DebugMonitor, this, 2000);
#else
  _is_debug_enable = true;
#endif

  _is_inited = true;
  return true;
}

CLogger& CLogger::Get(void)
{
  static CLogger CLogger;
  return CLogger;
}

void CLogger::Shutdown(void)
{
  spdlog::shutdown();
}

void CLogger::Log(const spdlog::source_loc& loc, spdlog::level::level_enum lvl,
    const char *data)
{
  spdlog::string_view_t name_("spdlog", 6);
  spdlog::details::log_msg msg(loc, name_, lvl,
      spdlog::string_view_t(data, strlen(data)));
  if (lvl == spdlog::level::trace)
    daily_trace->log(msg);
  else if (lvl == spdlog::level::debug) {
    if (_is_debug_enable)
      daily_debug->log(msg);
  }
  else if (lvl == spdlog::level::info)
    daily_info->log(msg);
  else if (lvl == spdlog::level::warn)
    daily_warn->log(msg);
  else if (lvl == spdlog::level::err)
    daily_error->log(msg);
  else if (lvl == spdlog::level::critical)
    daily_critical->log(msg);
}

spdlog::level::level_enum CLogger::Level(void)
{
  return _log_level;
}

void CLogger::SetLevel(spdlog::level::level_enum lvl)
{
  _log_level = lvl;
  spdlog::set_level(lvl);
}

void CLogger::SetFlushOn(spdlog::level::level_enum lvl)
{
  spdlog::flush_on(lvl);
}

const char* CLogger::GetShortName(std::string path)
{
  if (path.empty())
    return path.data();

  size_t pos = path.find_last_of("/\\");
  return path.data() + ((pos == path.npos) ? 0 : pos + 1);
}

void CLogger::DebugMonitor(void *p)
{
  if (!access("/tmp/log_debug", F_OK))
    _is_debug_enable = true;
  else
    _is_debug_enable = false;
}


#if 0 //def TEST
int main(int ac, char **av)
{
  CLogger::Get().Init("./log/", "test");
  AINFO << " test " << __FILE__ << " line " << __LINE__;
  ADEBUG << " test " << __FILE__ << " line " << __LINE__;
  PRINT_TRACE("printf trace test line %s %d", __FILE__, __LINE__);
  PRINT_DEBUG("printf debug test line %s %d", __FILE__, __LINE__);
  PRINT_INFO("printf info test line %s %d", __FILE__, __LINE__);
  PRINT_WARN("printf warn test line %s %d", __FILE__, __LINE__);
  PRINT_ERROR("printf error test line %s %d", __FILE__, __LINE__);
  CLogger::Get().Shutdown();
  return 0;
}
#endif
// g++ -std=c++11 -I. common/log.cc -Ithird_party/spdlog/include -Lthird_party/spdlog/lib -lspdlog_x86 -pthread
