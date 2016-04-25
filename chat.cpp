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
struct semaphore
{
	sem_t* rs;
	sem_t* ws;
	sem_t* ib;
};
void Server(semaphore sem,int fifo);
void write_str(semaphore sem,int fifo, std::string& str);
void read_str(semaphore sem,int fifo);
void end_seq(std::vector<pthread_t>& threads);

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
		/*if((lth && isStr) || (lth && !isStr && Sstr))
		{
			int* status;
			pthread_join(temp,(void**)&status);
			lth = false;
		}*/
		int val;
		if(!isStr)
		{
			sem_getvalue(sem.ws,&val);
			if(val == 0)
			{
				read_str(sem,fifo);
			}
		}
		if(isStr)
		{
			sem_getvalue(sem.ws,&val);
			if(val == 0)
			{
				read_str(sem,fifo);
			} else
			{
				if( sem_trywait(sem.ib) == 0)
				{
					sem_wait(sem.ws);
					write_str(sem,fifo,sent);
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
	}
}
void read_str(semaphore sem,int fifo)
{
	sem_wait(sem.rs);
	sem_wait(sem.ib);
    int len = 0;
    read(fifo, &len, sizeof(int));
    char *buf = new char[len];
    read(fifo, buf, len);
    std::cout <<"Other: " << buf << std::endl;
    delete buf;
    sem_post(sem.rs);
    sem_post(sem.ib);
}
void write_str(semaphore sem,int fifo, std::string& str)
{
	 int val = 1;
	 while(val == 1)
		 sem_getvalue(sem.rs,&val);
	 int len = str.length();
	 write(fifo, &len, sizeof(int));
	 write(fifo, str.c_str(), str.length() * sizeof(char));
	 sem_post(sem.ws);
	 sem_post(sem.ib);
}
void end_seq(std::vector<pthread_t>& threads)
{
	for(auto &th : threads)
	{
		int* status;
		pthread_join(th,(void**)&status);
	}
}
int main(int argc,char* argv[])
{
    std::string name = "fifo0001.12";
	semaphore sem;
	mkfifo(name.c_str(),0777);
	int fifo = open(name.c_str(),O_RDWR);
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
	Server(sem,fifo);

	return 0;
}
