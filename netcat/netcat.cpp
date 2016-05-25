#include <iostream>
#include <cstdio>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>

const int BUFSIZE = 1024;

char msg[BUFSIZE];
int sock_fd;

void CheckError( int errFlag )
{
    if (errFlag == -1)
    {
        std::cout << strerror(errno) << std::endl;
        exit(-1);    
    }
}
void CheckAddrError( int errCode )
{
    if(errCode != 0)
    {
        std::cout << errCode << std::endl;
        std::cout << gai_strerror(errCode);
        exit(-1);
    }
}
void CheckSocketError( int socketFd )
{
    if(socketFd == -1)
    {
        std::cout << "failed to connect";
        exit(-1);
    }

}
std::string GetAll()
{
    std::string result = "";
    std::string temp;
    while(!std::cin.eof())
    {
        getline(std::cin,temp);
        result += temp;
        temp.clear();
    }
    return result;
}

int Connect(char* host,char* port)
{
	int socketFd = -1;
	addrinfo* result = 0;
	int errCode = getaddrinfo(host,port,NULL,&result);
	CheckAddrError(errCode);
	for(addrinfo* temp = result; temp != NULL; temp = temp->ai_next)
	{
		socketFd = socket(temp->ai_family,temp->ai_socktype,temp->ai_protocol);
		if(socketFd == -1)
			continue;
		if(connect(socketFd,temp->ai_addr,temp->ai_addrlen) != -1)
			break;
	}
	freeaddrinfo(result);
	return socketFd;
}

void* Server_query(void* arg)
{
	std::string query;
	while(1)
	{
		std::getline(std::cin,query);
		query += "\n";
		write(sock_fd,query.c_str(),query.size());
	}
}
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << " Wrong input format" << std::endl ;
        exit(-1);
    }
   // std::string command = GetAll();
    sock_fd = Connect(argv[1],argv[2]);
    CheckSocketError(sock_fd);
    pthread_t thread;
    pthread_create(&thread,NULL,Server_query,NULL);
    int len;
    while ((len = recv(sock_fd, msg, BUFSIZE, 0)) > 0)
    {
    	for (int i = 0; i < len; i++)
    		putchar(msg[i]);
    }
    pthread_join(thread,NULL);
    return 0;

}



