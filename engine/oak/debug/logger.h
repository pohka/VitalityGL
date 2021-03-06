#pragma once
#ifndef LOGGER_H
#define LOGGER_H

#include <oak/build_def.h>


#include <string>
#ifdef DEBUG_MODE
  #include <iostream>
#endif
#include <vector>
#include <iomanip>
#include <oak/time/time.h>
#include <oak/core/types.h>

namespace debug
{
  cnum DEBUG_LOG_MSG = 0;
  cnum DEBUG_LOG_WARNING = 1;
  cnum DEBUG_LOG_ERROR = 2;
  //cnum DEBUG_LOG_LUA = 3;

  //Debug logger
  struct LogData
  {
    std::string funcName;
    std::string file;
    int line;
    uchar logType;
  };

  class Logger
  {
    uchar logType;

    struct Timer
    {
      float startTime;
      std::string name;
      std::string msg;
    };
    
    static std::vector<Timer> timers;
    
    
    static LogData lastLog;
    static uint logRepeatCount;
    static const uint MAX_REPEATED_LOGS = 10;
    static bool hasReachedMaxRepeats;

    public:
      Logger(
        const std::string& funcName, 
        const std::string& file, 
        int line, 
        uchar logType
      );
      ~Logger();

      static void startTimer(const std::string& name, const std::string& msg);
      static void endTimer(const std::string& name);

      //print the next value
      template <class T>
      Logger &operator<<(const T &v)
      {
#ifdef DEBUG_MODE
        if (!hasReachedMaxRepeats)
        {
          std::cout << v;
        }
#endif
        return *this;
      }
  private:
    static bool isRepeatedLog(
      const std::string& funcName,
      const std::string& file,
      int line,
      uchar logType
    );
  };

  
}


#define LOG debug::Logger(__FUNCTION__, __FILE__, __LINE__, debug::DEBUG_LOG_MSG)
#define LOG_WARNING debug::Logger(__FUNCTION__, __FILE__, __LINE__, debug::DEBUG_LOG_WARNING)
#define LOG_ERROR debug::Logger(__FUNCTION__, __FILE__, __LINE__, debug::DEBUG_LOG_ERROR)
//#define LOG_LUA debug::Logger(__FUNCTION__, __FILE__, __LINE__, debug::DEBUG_LOG_LUA)
//#define LOG_LUA_ERROR  debug::Logger(__FUNCTION__, __FILE__, __LINE__, debug::DEBUG_LOG_LUA_ERROR)


#endif