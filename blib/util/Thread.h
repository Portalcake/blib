#pragma once

#include <blib/config.h>

#ifdef BLIB_WIN
#include <windows.h>
#include <map>
#else
#include <pthread.h>
#endif

#include <string>

namespace blib
{
	namespace util
	{
		class Thread
		{
		private:
#ifdef BLIB_WIN
			HANDLE	threadHandle;
			static DWORD WINAPI threadStarter( LPVOID lpParam );
			static std::map<int, std::string> threadNames;
#else
			pthread_t thread;
			static void* threadStarter(void* param);
#endif
		protected:
			bool running;
		public:
			Thread(std::string);
			virtual ~Thread();
			void start();
			void stop();
			void kill();
			void signal();
			int getId();
			void waitForTermination();
			static  void sleep(int);
			virtual int run() = 0;

			std::string name;
			static std::string getCurrentThreadName();
			static void setMainThread();
			static void setThreadName(const std::string &name);
#ifdef WIN32
			unsigned long threadId;
			friend void SetThreadName(DWORD dwThreadID, const char* threadName);
#endif
		};


	}
}