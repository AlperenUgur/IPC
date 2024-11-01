#include <csignal>
#include <iostream>
#include <thread>

using namespace std;

bool flag = false;
bool threadFlag = false;
void signalHandler(int signum)
{
	cout << "\n interrup signal(" << signum << ") received. \n";
	flag = true;
	threadFlag = true;
};
void threadFun()
{
	while(threadFlag == false)
	{
		cout << "thread is running" << endl;
		sleep(1);
	}
}

int main()
{
	string text;
	fd_set read_fds;
	struct timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	thread thread1(threadFun);
	signal(SIGINT, signalHandler);
	while(flag == false)
	{
		cout << "while started\n" << endl;
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		int result = select(STDIN_FILENO + 1, &read_fds, nullptr, nullptr, &timeout);
		if(result > 0 && FD_ISSET(STDIN_FILENO, &read_fds))
		{
			cout << "enter a text" << endl;
			cin >> text;
		}
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
	}
	cout << "program is finished succesfully" << endl;
	thread1.join();
	return 0;
}