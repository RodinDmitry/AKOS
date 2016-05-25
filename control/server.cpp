#include <pwd.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <semaphore.h>
#include <queue>
const char* hostname = "localhost";
const unsigned int port_number = 37600;


bool end_flag = false;
int server_sockfd = 0;


vector<int> consoles;
map<int,int> idfd;
vector<pthread_t> threads;
sem_t* initsem;
sem_t* waitsem;
pthread_t getThread;
void Server();
void* getmsg(void* arg);

void* getmsg(void* arg)
{
	while(!end_flag)
		{
			int type = 0
			int len = 0;
			int id = 0;
			read(sockfd,&len,sizeof(int));
			read(sockfd,&id,sizeof(int));
			char* buf = new char[len];
			read_ret = read(sockfd,buf,sizeof(char) * len);
			std::string temp(buf);
			ParsQuery(temp,id);
			delete[] buf;
		}
	pthread_exit(0);
}
void ParsQuery(string query,int id)
{
	switch (query)
	{
		case "Shutdown":
		{
			end_flag = true;
			break;
		}
		case "Log":
		{
			GetLog(id);
			break;
		}
		case "Reread"
		{
			Rerun();
			break;
		}
	}
}
void msg_send(string msg,int id)
{
	int type = 0;
	int fd = idfd[id];
	int len = msg.size();
	write(fd, (void*)&type, sizeof(int));
	write(fd, (void*)&len, sizeof(int));
	write(fd, msg.c_str(), sizeof(char)*len);
	return 0;
}
void Server()
{
	printf("Server started\n");


    struct sockaddr_in server_address;
    server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((short)port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;
    int bind_res = bind(server_sockfd, (struct sockaddr*)(&server_address), sizeof(struct sockaddr_in));
    if (bind_res == -1)
    {
    	printf("Fail to bind\n");
    	return;
    }
    listen(server_sockfd, 10);

    initsem = sem_open("/semInit",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
    waitsem = sem_open("/semWait",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	pthread_create(&getThread,NULL,getmsg,NULL);
	int curid = 0;
    while(!end_flag)
    {
    	int temp_sock = accept(server_sockfd,NULL,NULL);
    	sem_wait(waitsem);
    	sem_wait(initsem);
    	idfd[curid] = temp_sock;
    	curid++;
    	pthread_t thread;
    	threads.push_back(thread);
    	consoles.pish_back(temp_sock);
    }
}
