#pragma once

#include <vector>
#include <string>

#include "Singleton.h"

namespace Engine
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    struct LogEntry
    {
        LogLevel level;
        std::string message;
    };

	class LogManager : public Singleton<LogManager>
    {
    public:
        static LogManager& GetInstance()
        {
            static LogManager instance;
            return instance;
        }

        void Add(LogLevel level, const std::string& msg)
        {
            m_entries.push_back({ level, msg });

            if (m_entries.size() > 1000)
            {
                m_entries.erase(m_entries.begin());
            }
        }

        const std::vector<LogEntry> GetEntries() const
        {
            return m_entries;
        }

        void Clear()
        {
            m_entries.clear();
        }

    private:
        std::vector<LogEntry> m_entries;
    };
}