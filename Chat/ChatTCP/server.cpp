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
const unsigned int port_number = 33549;


bool end_flag = false;
bool start_flag = false;
int server_sockfd = 0;

std::queue<std::string> msg_queue;
std::vector<int> clients;
std::vector<pthread_t> threads;
std::map<int,std::string> names;
sem_t* init_sem;
pthread_t br_th;
void Server();
void* Client_read(void* arg);
void* Clients_broadcast(void* arg);

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
	init_sem = sem_open("/sem_init",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	pthread_create(&br_th,NULL,Clients_broadcast,NULL);
    int sockfd;
    while(!end_flag)
    {
    	sockfd = accept(server_sockfd, NULL, NULL);
    	sem_wait(init_sem);
    	clients.push_back(sockfd);
    	names[sockfd] = "Unknown";
    	pthread_t thread;
    	threads.push_back(thread);
    	pthread_create(&thread,NULL,Client_read,NULL);
    }
}
void* Client_read(void* arg)
{
	int sockfd = clients[clients.size()-1];
	sem_post(init_sem);
	while(!end_flag)
	{
			int type = 0;
			// type == 0  - standart message
			// type == 1  - name set
			int len = 0;
			read(sockfd,&type,sizeof(int));
			read(sockfd,&len,sizeof(int));
			char* buf = new char[len];
			read(sockfd,buf,sizeof(char) * len);
			std::string temp(buf);
			if(type == 1)
			{
				names[sockfd] = temp;
				delete[] buf;
				continue;
			}
			msg_queue.push(names[sockfd] + ": " + temp);
			delete[] buf;
	}
	pthread_exit(0);
}
void* Clients_broadcast(void* arg)
{
	while(!end_flag)
		{
			if(msg_queue.size() != 0)
			{
				std::string msg = msg_queue.front();
				sem_wait(init_sem);
				for(int  i = 0;i < clients.size();++i)
				{
					int len = msg.size();
					write(clients[i], (void*)&len, sizeof(int));
					write(clients[i], msg.c_str(), sizeof(char)*len);
					msg_queue.pop();
				}
				sem_post(init_sem);
			}
		}
	pthread_exit(0);
}
int main()
{
	Server();
}
