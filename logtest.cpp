#include <boost/thread.hpp>
#include <boost/date_time.hpp>

#include <iostream>

#include "log/Log.hpp"

using namespace std;

LOG_INIT;

void threadLog()
{

	boost::posix_time::seconds workTime(3);
	boost::this_thread::sleep(workTime);
	LOG_MSG << fmt("% %") % "Thread" % boost::this_thread::get_id();
	LOG_MSG << "Hallo" << 1 << 2 << 3 << 4;

}

int main(int ac, char* av[])
{
	LOG_MSG << "LogTestApp. -- written by Wilston Oreo.";

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
	LOG_WRN << "Warning";
	LOG_MSG_(3);
	LOG_MSG_(4);

	LOG_ERR << "ERROR";

	cout << endl;
	return 0;
}

