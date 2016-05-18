#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

class num_mem
{
public:

	num_mem()
	{
		cm = sem_open("/member1",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,2);
		int val;
		sem_getvalue(cm,&val);
		if(val == 0)
		{
			printf("Chat system: Unable to join a chat\n");
		}
		sem_wait(cm);
		printf("Chat system: Joinned the chat\n");
	}
	~num_mem()
	{
		sem_post(cm);
		printf("You have left the chat\n");
	}
	int getval()
	{
		int val;
		sem_getvalue(cm,&val);
		return val;
	}
private:
	sem_t* cm;
};
struct data
{
	sem_t* rs;
	sem_t* ws;
	sem_t* ib;
	num_mem* member;
	bool AbortFlag;
	pthread_t thread;
} pdata;
void Server(int fifo);
int write_str(int fifo, std::string& str);
int read_str(int fifo);
void end_seq(pthread_t thread);
//void clear(pthread_t thread);

bool EF = false;
bool isStr = false;
bool lth = false;
std::string sent;

void* getStr(void * arg)
{
	while (true)
	{
		if(!isStr && lth)
		{
			std::getline(std::cin,sent);
			if(sent == "Goodbye")
			{
				EF = true;
				pthread_exit(0);
			} else
			{
				isStr = true;
			}
			lth = false;
		}
		if(pdata.AbortFlag)
			pthread_exit(0);
	}
}

void Server(int fifo)
{
	bool flag = true;
	lth = false;
	pthread_create(&pdata.thread,NULL,getStr,NULL);
	while (flag)
	{
		if(!lth && !isStr)
		{
			lth = true;
		}
		int val;
		if(!isStr)
		{
			sem_getvalue(pdata.ws,&val);
			if(val == 0)
			{
				read_str(fifo);
			}
		}
		if(isStr)
		{
			sem_getvalue(pdata.ws,&val);
			if(val == 0)
			{
				read_str(fifo);
			} else
			{
				if( sem_trywait(pdata.ib) == 0)
				{
					sem_wait(pdata.ws);
					int status = write_str(fifo,sent);
					isStr = false;
				}
			}
		}
		if(EF)
		{
			flag = false;
			end_seq(pdata.thread);
			return;
		}
	}
}
int read_str(int fifo)
{
	sem_wait(pdata.rs);
	int status = sem_trywait(pdata.ib);
	while(status == -1)
	{
		if(pdata.member->getval() == 1)
		{
			sem_post(pdata.rs);
			 printf("Chat system: No other members\n");
			return -1;
		}
		status = sem_trywait(pdata.ib);

	}
    int len = 0;
    read(fifo, &len, sizeof(int));
    char *buf = new char[len];
    read(fifo, buf, len);
    std::cout <<"Other: " << buf << std::endl;
    delete buf;
    sem_post(pdata.rs);
    sem_post(pdata.ib);
    return 0;
}
int write_str(int fifo, std::string& str)
{
	 int val = 1;
	 while(val == 1)
	 {
		 sem_getvalue(pdata.rs,&val);
		 if(pdata.member->getval() == 1)
		 {
			 sem_post(pdata.ws);
			 sem_post(pdata.ib);
			 printf("Chat system: No other members\n");
			 return -1;
		 }
	 }
	 int len = str.length();
	 write(fifo, &len, sizeof(int));
	 write(fifo, str.c_str(), str.length() * sizeof(char));
	 sem_post(pdata.ws);
	 sem_post(pdata.ib);
	 return 0;
}

void end_seq(pthread_t thread)
{
	int* status;
	pthread_join(thread,(void**)&status);
	delete pdata.member;
}
void AbortHandler(int signum)
{
	pdata.AbortFlag = true;
	end_seq(pdata.thread);
	exit(0);
}
void setAbortHandler()
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = AbortHandler;
	sigset_t   set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	act.sa_mask = set;
	sigaction(SIGINT, &act, 0);
}

void init_sem()
{
	int val;
	pdata.rs = sem_open("/rs3",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	sem_getvalue(pdata.rs,&val);
		if(val != 1)
			sem_post(pdata.rs);
	pdata.ib = sem_open("/ib3",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	sem_getvalue(pdata.ib,&val);
		if(val != 1)
			sem_post(pdata.ib);
	pdata.ws = sem_open("/ws3",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	sem_getvalue(pdata.ws,&val);
		if(val != 1)
			sem_post(pdata.ws);
	pdata.member = new num_mem;
	pdata.AbortFlag = false;
}
int main(int argc,char* argv[])
{
	setAbortHandler();
    std::string name = "fifo0001.12";
	mkfifo(name.c_str(),0777);

	int fifo = open(name.c_str(),O_RDWR);
	init_sem();
	Server(fifo);

	return 0;
}
