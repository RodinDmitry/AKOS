#include <pwd.h>
#include <syslog.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
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


const char* hostname = "localhost";
const unsigned int port_number = 33549;

bool have_msg = false;
std::string msg;
bool end_flag = false;
int sockfd = 0;
pthread_t thread_msg;
pthread_t thread_read;
bool start_flag = false;
int server_sockfd = 0;
bool IsClient = false;

void Server();
void* get_msg(void* arg);
int msg_send(int desk);
void end_seq();
void* msg_read(void* arg);

void AbortHandler(int signum)
{
	end_seq();
	printf("Process aborted\n");
	exit(-1);
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

void Server()
{
	printf("Server started\n");
	IsClient = false;
    struct sockaddr_in server_address;
    server_sockfd = socket(PF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((short)port_number);
    server_address.sin_addr.s_addr = INADDR_ANY;
    int bind_res = bind(server_sockfd, (struct sockaddr*)(&server_address), sizeof(struct sockaddr_in));
    if (bind_res == -1)
    {
    	printf("Fail to bind\n");
    }
    listen(server_sockfd, 1);


	pthread_create(&thread_msg,NULL,get_msg,NULL);
	pthread_create(&thread_read,NULL,msg_read,NULL);
	sockfd = accept(server_sockfd, NULL, NULL);
	printf("Client joined\n");

    while(1)
    {
    	start_flag = true;
    	if(have_msg)
    	{
    		msg_send(sockfd);
    	}
    	if(end_flag)
    	{
    		end_seq();
    		return;
    	}
    }
}
void Client()
{
	printf("Client started\n");
	IsClient = true;
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

    pthread_create(&thread_msg,NULL,get_msg,NULL);
    pthread_create(&thread_read,NULL,msg_read,NULL);

    while(1)
    {
    	start_flag = true;
    	if(have_msg)
    	{
    	    msg_send(sockfd);
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
			if (read_ret == 0)
			{
				continue;
			}
			char* buf = new char[len];
			read_ret = read(sockfd,buf,sizeof(char) * len);
			if (read_ret == 0)
			{
				continue;
			}
			std::string temp(buf);
			std::cout <<"Other: " << buf << std::endl;
			if(temp == "Goodbye")
			{
				printf("Other disconnected\n");
				if(IsClient)
				{
					end_flag = true;
				}
			}
			delete[] buf;
		}
	}
	pthread_exit(0);
}
int msg_send(int desk)
{
	int len = msg.size();
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
int main(int argc,char* argv[])
{
	setAbortHandler();
	if(argc == 1)
	{
		printf("Not enough arguments\n");
		return -1;
	}
	if(argv[1][0] == 'S')
	{
		Server();
	} else
	{
		Client();
	}

}
