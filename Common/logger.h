#ifndef _STRATCOMMON_LOGGER_H_
#define _STRATCOMMON_LOGGER_H_
#include <sys/time.h>
#include <iostream>
#include <iomanip>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		START : Logger.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace FlexStrategy
{
static const char* Time()
{
	static char timeString[128] = "";
	static struct tm *mytm;
	struct timeval tv;

	gettimeofday(&tv, 0);
	mytm = localtime(&tv.tv_sec);

	sprintf(timeString,"%02d:%02d:%02d.%06ld", mytm->tm_hour, mytm->tm_min, mytm->tm_sec, tv.tv_usec);
	return timeString;
}

class LogScope
{
        public:
                static std::string m_str;

                LogScope(const char* str)
                {
                        if(str)
                                m_str = str;
                        else
                                m_str = "";
                }

                ~LogScope()
                {
                        m_str = "";
                }

                static std::string Msg()
                {
                        return m_str;
                }
};
}; // namespace FlexStrategy

// Common macros for strategy log prints
#define STRAT_DEBUG_LEVEL	(0)  // 0 - Info; 1 - Verbose; Else - Full.

#if (STRAT_DEBUG_LEVEL == 0 )			// 0 - Info
	#define _STRAT_LOG_VERB_(a)
	#define _STRAT_LOG_DBUG_(a)
#else
	#if	(STRAT_DEBUG_LEVEL == 1 )		// 1 - Verbose
		#define _STRAT_LOG_VERB_(a)	a
		#define _STRAT_LOG_DBUG_(a)
	#else								// Else - Full
		#define _STRAT_LOG_VERB_(a)	a
		#define _STRAT_LOG_DBUG_(a)	a
	#endif
#endif

#define CERR	StrategyBase::m_Log << "[" << FlexStrategy::Time() << "] ERROR  [" << setw(24) << __func__ << "::" << setw(4) << __LINE__ << "] "
#define CWARN	StrategyBase::m_Log << "[" << FlexStrategy::Time() << "] WARN   [" << setw(24) << __func__ << "::" << setw(4) << __LINE__ << "] "
#define COUT	StrategyBase::m_Log << "[" << FlexStrategy::Time() << "] OUTPUT [" << setw(24) << __func__ << "::" << setw(4) << __LINE__ << "] "
#define CINFO	StrategyBase::m_Log << "[" << FlexStrategy::Time() << "] INFO   [" << setw(24) << __func__ << "::" << setw(4) << __LINE__ << "] "
#define CDEBUG	StrategyBase::m_Log << "[" << FlexStrategy::Time() << "] DEBUG  [" << setw(24) << __func__ << "::" << setw(4) << __LINE__ << "] "

#endif // #ifndef _STRATCOMMON_LOGGER_H_
