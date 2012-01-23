/************************FreeBSD license header*****************************
 * Copyright (c) 2012, Wilston Oreo
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***********************************************************************/


#pragma once 
#define LOGGING

#define OUTPUT_TIME
#define OUTPUT_COLOR
#define OUTPUT_MSG
#define OUTPUT_SRC
#define THREAD_SAFE

#include <iostream>
#include <sstream>
#include <vector>
#include <time.h>

#ifdef THREAD_SAFE
#include <boost/thread/mutex.hpp>
#endif 

using namespace std;

/// @brief: Simple struct for formatting strings with parameters
struct fmt 
{
	fmt(const string& s) { this->operator()(s); }
	fmt(const char* c)   { this->operator()(c); }

	void operator()(const char* c)
	{
		this->operator()(string(c));
	}

	void operator()(const string& s) 
	{ 
		nParams = 0; buf = s; size_t n = 0;
		for (size_t i = 0; i < s.length(); i++)
			if (s[i] == '%')
			{
				if (i > 0) 
					if (s[i-1] == '\\') continue;
				n++;
			}
		params.resize(n);
	}

	template<class T> fmt& operator%(const T &t) 
	{
		if (nParams < params.size())
		{
			stringstream ss; ss << t;
			params[nParams] = ss.str();
			nParams++;
		}
		return *this;
	}

	string str()
	{ 
		stringstream ss; size_t n = 0; 
		for (size_t i = 0; i < buf.length(); i++)
		{
			char c = buf[i];
			switch (c)
			{
				case '%':
					if (i > 0) 
						if (buf[i-1] == '\\') continue;
					if (n >= params.size()) continue;
					ss << params[n]; n++;
					break;
				case '\\':
					if (i < buf.length()-1)
						if (buf[i+1]=='%') 
						{ ss << '%'; continue; }
				default:
					ss << c;
			}
		}
		return ss.str(); 
	}

	void reset() 
	{ 
		nParams = 0;  
		for (size_t i = 0; i < params.size(); i++)
			params[i] = "";
	}
	private:
	string buf;
	vector<string> params;
	size_t nParams;
};

/// @brief: Singleton class for Logging
class Log 
{
	int level_,curLevel;
	ostream* output_;
	static bool destroyed_;
	static Log *_instance;
	Log() { init(); };
	Log(const Log&) {}

	Log& operator=(const Log&) { return *this; }
	~Log()
	{
		_instance = 0;
		destroyed_ = true;
	}

	static void create()
	{
		static Log theInstance;
		_instance = &theInstance;
	}

	static void onDeadReference()
	{
		create();
		new(_instance) Log;

#ifdef ATEXIT_FIXED
		atexit(killPhoenixLog);
#endif
		destroyed_ = false;
	}

	static void killPhoenixLog()
	{
		_instance->~Log();
	}

#ifdef THREAD_SAFE
	boost::mutex mutex; 
#endif

	void lock()
	{
#ifdef THREAD_SAFE
		mutex.lock();
#endif
	}

	void unlock()
	{
#ifdef THREAD_SAFE
		mutex.unlock();
#endif
	}

	template<class T> string colorStr(int color, int bold, char delimiter, T t)
	{
		stringstream ss;
#ifdef OUTPUT_COLOR
		if (output_ == &cout)
			ss << "\033[" << bold << ";" << color << "m";
#endif
		ss << t;
#ifdef OUTPUT_COLOR
		if (output_ == &cout) ss << "\033[0m";
#endif
		ss << delimiter;
		return ss.str();
	}


	string logHeader(string& file, string& function, 
			int linenumber, int _level, string& type)
	{
#ifdef LOGGING
		stringstream ss; 
		int color = 39;
#ifdef OUTPUT_COLOR
		if (type=="ERR") color = 31; 
		if (type=="MSG") color = 37; 
		if (type=="WRN") color = 33;
#endif 
#ifdef OUTPUT_TIME
		time_t rawtime;
		struct tm * timeinfo;
		char timeBuffer[30];
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		strftime(timeBuffer,30,"%Y/%m/%d-%H:%M:%S",timeinfo); 
		ss << colorStr(color,0,'|',timeBuffer);		
#endif
	
#ifdef OUTPUT_MSG
		ss << colorStr(color,1,'|',type);
		ss << colorStr(color,1,'|',_level);
#endif

#ifdef OUTPUT_SRC
		int pPos = 0;
		for (int i = file.length()-1; i > 0; i--)
			if (file.substr(i,1)==string("/"))
			{ pPos = i+1; break; }
		file = file.substr(pPos,file.length()-pPos);
		ss << colorStr(35,0,':',file);
		ss << colorStr(32,1,':',linenumber);
		ss << colorStr(36,0,'|',function);
#endif

		return ss.str();
#endif
#ifndef LOGGING		
		return string();
#endif
	}


public:
	void init()
	{
		level(1); 
		output(&cout);
	}
	ostream* output() { return output_; }
	void output(ostream* _output) { output_ = _output; }

	int level() { return level_; }
	void level(int _level)
	{
		level_ = (_level > 10) ? 10 : 
			(_level < 0) ? 0 : _level;  
	}

	Log& log(string file, string function, 
			int linenumber, int _level, string type)
	{
#ifdef LOGGING
		curLevel = _level;
		if (curLevel > level_) return *this;  

		lock();
		(*output_) << endl; 
		(*output_) << logHeader(file,function,linenumber,_level,type); 
		unlock();
#endif
		return *this;
	}

	Log& operator<< (fmt& f)
	{
#ifdef LOGGING
		if (curLevel <= level_)
		{ 
			lock();
			(*output_) << f.str(); f.reset(); 
			unlock();
		}
#endif
		return *this;
	}
	template <typename T> Log& operator<< (const T& t) 
	{
#ifdef LOGGING
		if (curLevel <= level_)  
		{
			lock();
			(*output_) << t;
			unlock();
		}
#endif
		return *this; 
	}

	static Log* instance()
	{
		if (!_instance)
		{
			if (destroyed_) 
				onDeadReference();
			else 
				create();
		}
		return _instance;
	}
};

#define LOG Log::instance() 
#define LOG_INIT Log *Log::_instance = 0; bool Log::destroyed_ = false;

#define LOG_(lvl,type) LOG->log(__FILE__,__FUNCTION__,__LINE__,(lvl),(type))

#define LOG_MSG LOG_(0,"MSG")
#define LOG_MSG_(lvl) LOG_((lvl),"MSG")
#define LOG_ERR LOG_(0,"ERR")
#define LOG_WRN LOG_(0,"WRN")
#define LOG_WRN_(lvl) LOG((lvl),"WRN") 

