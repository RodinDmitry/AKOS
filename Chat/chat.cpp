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
class num_mem
{
public:

	num_mem()
	{
		cm = sem_open("/mem",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,2);
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
struct semaphore
{
	sem_t* rs;
	sem_t* ws;
	sem_t* ib;
};

void Server(semaphore sem,int fifo);
int write_str(semaphore sem,int fifo, std::string& str,num_mem* member);
int read_str(semaphore sem,int fifo,num_mem* member);
void end_seq(std::vector<pthread_t>& threads);
void clear(std::vector<pthread_t>& threads);

bool EF = false;
bool isStr = false;
bool lth = false;
std::string sent;

void* getStr(void * arg)
{
	std::getline(std::cin,sent);
	if(sent == "Goodbye")
	{
		EF = true;
	} else
	{
		isStr = true;
	}
	lth = false;
    pthread_exit(0);
}

void Server(semaphore sem,int fifo)
{
	num_mem member;
	bool flag = true;
	lth = false;
	std::vector<pthread_t> threads;
	while (flag)
	{
		pthread_t temp;
		if(!lth && !isStr)
		{
			lth = true;
			std::string* ptr = &sent;
			pthread_create(&temp,NULL,getStr,NULL);
			threads.push_back(temp);
		}
		int val;
		if(!isStr)
		{
			sem_getvalue(sem.ws,&val);
			if(val == 0)
			{
				read_str(sem,fifo,&member);
			}
		}
		if(isStr)
		{
			sem_getvalue(sem.ws,&val);
			if(val == 0)
			{
				read_str(sem,fifo,&member);
			} else
			{
				if( sem_trywait(sem.ib) == 0)
				{
					sem_wait(sem.ws);
					int status = write_str(sem,fifo,sent,&member);
					isStr = false;
				}
			}
		}
		if(EF)
		{
			flag = false;
			end_seq(threads);
			return;
		}
		if(threads.size() > 100)
		{
			clear(threads);
		}
	}
}
int read_str(semaphore sem,int fifo,num_mem* member)
{
	sem_wait(sem.rs);
	int status = sem_trywait(sem.ib);
	while(status == -1)
	{
		if(member->getval() == 1)
		{
			sem_post(sem.rs);
			 printf("Chat system: No other members\n");
			return -1;
		}
		status = sem_trywait(sem.ib);

	}
    int len = 0;
    read(fifo, &len, sizeof(int));
    char *buf = new char[len];
    read(fifo, buf, len);
    std::cout <<"Other: " << buf << std::endl;
    delete buf;
    sem_post(sem.rs);
    sem_post(sem.ib);
    return 0;
}
int write_str(semaphore sem,int fifo, std::string& str,num_mem* member)
{
	 int val = 1;
	 while(val == 1)
	 {
		 sem_getvalue(sem.rs,&val);
		 if(member->getval() == 1)
		 {
			 sem_post(sem.ws);
			 sem_post(sem.ib);
			 printf("Chat system: No other members\n");
			 return -1;
		 }
	 }
	 int len = str.length();
	 write(fifo, &len, sizeof(int));
	 write(fifo, str.c_str(), str.length() * sizeof(char));
	 sem_post(sem.ws);
	 sem_post(sem.ib);
	 return 0;
}
void clear(std::vector<pthread_t>& threads)
{
	pthread_t active = threads.back();
	for(auto &th : threads)
	{
		int* status;
		if(th != active)
		pthread_join(th,(void**)&status);
	}
	threads.clear();
	threads.push_back(active);
}
void end_seq(std::vector<pthread_t>& threads)
{
	for(auto &th : threads)
	{
		int* status;
		pthread_join(th,(void**)&status);
	}
}
void init_sem(semaphore& sem)
{
	int val;
	sem.rs = sem_open("/rs3",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	sem_getvalue(sem.rs,&val);
		if(val != 1)
			sem_post(sem.rs);
	sem.ib = sem_open("/ib3",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	sem_getvalue(sem.ib,&val);
		if(val != 1)
			sem_post(sem.ib);
	sem.ws = sem_open("/ws3",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	sem_getvalue(sem.ws,&val);
		if(val != 1)
			sem_post(sem.ws);
}
int main(int argc,char* argv[])
{
    std::string name = "fifo0001.12";
	semaphore sem;
	mkfifo(name.c_str(),0777);
	int fifo = open(name.c_str(),O_RDWR);
	int val;
	init_sem(sem);
	Server(sem,fifo);

	return 0;
}
