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

static const char* hostname = "localhost";
static const unsigned int port_number = 33549;

static bool have_msg = false;
static std::string msg;
static bool end_flag = false;
static int sockfd = 0;
static pthread_t thread_msg;
static pthread_t thread_read;
static bool start_flag = false;
static int server_sockfd = 0;

static void* get_msg(void* arg);
static int msg_send(int type,int desk);
static void end_seq();
static void* msg_read(void* arg);

void Client()
{
	struct hostent *hst;
	struct sockaddr_in server_addr;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
    hst = gethostbyname(hostname);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons((short)port_number);
    memcpy(&(server_addr.sin_addr.s_addr), hst->h_addr_list[0], hst->h_length);
    if (connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr_in)) == -1)
    {
       	printf("connect fail\n");
       	close(sockfd);
       	exit(0);
    }
    std::cout << "enter name" << std::endl;
    std::cin >> msg;
    msg_send(1,sockfd);
    pthread_create(&thread_msg,NULL,get_msg,NULL);
    pthread_create(&thread_read,NULL,msg_read,NULL);

    while(1)
    {
    	start_flag = true;
    	if(have_msg)
    	{
    	    msg_send(0,sockfd);
    	}
    	if(end_flag)
    	{
    		end_seq();
    		return;
    	}
    }
}


void* get_msg(void* arg)
{
	while(!end_flag)
	{
		if(start_flag)
		{
			if(!have_msg)
			{
				msg.clear();
				std::getline(std::cin,msg);
				have_msg = true;
				if(msg == "Goodbye")
					end_flag = true;
			}
		}
	}
	pthread_exit(0);
}
void* msg_read(void* arg)
{
	while(!end_flag)
	{
		if(start_flag)
		{
			int len = 0;
			ssize_t read_ret = read(sockfd,&len,sizeof(int));
			char* buf = new char[len];
			read_ret = read(sockfd,buf,sizeof(char) * len);
			std::string temp(buf);
			std::cout << buf << std::endl;
			if(buf == "Server out")
			{
				end_flag = true;
			}
			delete[] buf;
		}
	}
	pthread_exit(0);
}
int msg_send(int type,int desk)
{
	int len = msg.size();
	write(desk, (void*)&type, sizeof(int));
	write(desk, (void*)&len, sizeof(int));
	write(desk, msg.c_str(), sizeof(char)*len);
	have_msg = false;
	return 0;
}
void end_seq()
{
	pthread_join(thread_msg,NULL);
	pthread_join(thread_read,NULL);
	close(sockfd);
	close(server_sockfd);
}
int main()
{
	Client();
}
