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

#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include <time.h>

#include <iostream>

#include "log/Log.hpp"

using namespace std;

LOG_INIT;

void threadLog()
{
	boost::posix_time::seconds workTime(1);
	boost::this_thread::sleep(workTime);

	boost::thread::id tId = boost::this_thread::get_id();


	LOG_MSG << fmt("% %") % "Thread" % tId;
	LOG_MSG << "Hallo" << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8;
	boost::this_thread::sleep(workTime);
	LOG_MSG << fmt("% %") % "2 Thread" % tId;

	boost::posix_time::milliseconds mTime(rand()*100000/RAND_MAX);

	for (int i = 0; i < 100; i++)
	{
		LOG_MSG << fmt("%: % % %") % tId % i % (i*2) % (i*4);
		boost::this_thread::sleep(mTime);
	}
}

int main(int ac, char* av[])
{
	LOG_MSG << "LogTestApp. -- written by Wilston Oreo.";

	srand ( time(NULL) );

	LOG_MSG_(1) << "Startup";  
	boost::thread thread1(threadLog);
	boost::thread thread2(threadLog);
	boost::thread thread3(threadLog);
	boost::thread thread4(threadLog);
	LOG_MSG_(1) << "Waiting for threads...";

	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();

	LOG_MSG_(1) << "Done";

	LOG->level(2);

	LOG_MSG_(1) << fmt("Log level: %") % LOG->level();
	LOG_WRN << fmt("Warning %") % 3;
	LOG_MSG_(3);
	LOG_MSG_(4);

//	map<boost::thread::id, string> threadBuffers;
/*
	if (!threadBuffers.count(boost::this_thread::get_id()))
	{
		stringstream ss;
		boost::thread::id id = boost::this_thread::get_id();
		threadBuffers.insert(pair<boost::thread::id,string>(id,ss.str()));
	}*/
 //	stringstream ss; ss << &threadBuffers[boost::this_thread::get_id()];

	//ss << "Test";

//	cout << ss;

//	LOG_ERR << "ERROR";
	return 0;
}

