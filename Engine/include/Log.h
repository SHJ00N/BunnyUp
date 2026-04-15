#pragma once
#include <cstdarg>
#include <cstdio>
#include "LogManager.h"

inline std::string FormatString(const char* fmt, ...)
{
    char buffer[512];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    return std::string(buffer);
}

#define LOG_INFO(fmt, ...)  Engine::LogManager::GetInstance().Add(Engine::LogLevel::Info,    FormatString(fmt, ##__VA_ARGS__))
#define LOG_WARNING(fmt, ...)  Engine::LogManager::GetInstance().Add(Engine::LogLevel::Warning, FormatString(fmt, ##__VA_ARGS__))
#define LOG_ERROR(fmt, ...) Engine::LogManager::GetInstance().Add(Engine::LogLevel::Error,   FormatString(fmt, ##__VA_ARGS__))