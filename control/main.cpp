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
#include <set>

using namespace std;
struct procinf
{
	string proc;
	vector<string> param;
	procinf(string _proc): proc(_proc){};
};
char ** Pars_argv(procinf arg)
{
	char** ans = new char*[arg.param.size() + 1];
	ans[0] = const_cast<char *>(arg.proc.c_str());
	for(int i = 1;i <= arg.param.size();++i)
	{
		ans[i] = const_cast<char *>(arg.param[i-1].c_str());
	}
	return ans;
}
map<string,queue<procinf>> procL;
map<string,int> name_pid;
set<int> active;
set<int> pids;
set<string> launched;
set<string> delayed;

bool end_flag = false;
int countlaunched = 0;


pthread_t delayed;


void exec_proc(procinf arg)
{
	countlaunched++;
	char** argv = Pars_argv(arg);
	int pid = fork();
	if(pid == 0)
	{
		execv(argv[0],argv);
	}
	else
	{
		pids.insert(pid);
		status[pid] = 0;
		name_pid[arg.proc] = pid;
		active.insert(pid);
		launched.insert(arg.proc);
	}
}
procinf getnext()
{
	int num = 0;
	string name;
	int numarg = 0;
	vector<string> arg;
	int time;
	cin >> num >> name >> numarg;
	for(int i = 0;i<numarg;+++i)
	{
		string temp;
		cin >> temp;
		arg.push_back(temp);
	}
	cin >> time;
	procinf newinf(name);
	newinf.param = arg;
	sleep(time);
	return newinf;
}
int getstatus(int pid)
{
	int stat;
	if(active.find(pid) != active.end())
	{
		waitpid(pid,&stat,WNOHANG);
		if(WIFEXITED(stat))
		{
			//active.erase(pid);
			return 1;
		}
		if(WIFSIGNALED(stat))
		{
			//active.erase(pid);
			return 2;
		}
		return 3;
	}
	return 0;
}
void GetLog(int id)
{
	for(auto& pid : pids)
	{
		if(active.find(pid) != active.end())
		{
			int stat = getstatus(pid);
			switch (stat)
			{
				case 0 :
					{
						string temp("Ended");
						msg_send(temp,int id);
						break;
					}
				case 1 :
					{
						string temp("Exited");
						msg_send(temp,int id);
						break;
					}
				case 2 :
					{
						string temp("Terminated");
						msg_send(temp,int id);
						break;
					}
				case 3 :
					{
						string temp("Running");
						msg_send(temp,int id);
						break;
					}
			}
		}
	}
}
void SequenceRun(int num)
{
	for(int i = 0;i < num;++i)
	{
		if(end_flag)
			return;
		procinf inf = getnext();
		if(lauched.find(inf.proc) != launced.end())
		{
			int pid = name_pid[inf.proc];
			int stat = getstatus(pid);
			if(stat < 3)
			{
				exec_proc(inf);
			} else
			{
				procL[inf.proc].push(inf);
				delayed.insert(inf.proc);
			}
		}
	}
}
void* DelayedRun(void* arg)
{
	int* num = (int*)arg;
	while(!end_flag)
	{
		if(*num == countlaunched)
		{
			end_flag = true;
			continue;
		}
		for(auto& name : delayed)
		{
			if(procL[name].size() == 0)
			{
				continue;
			}
			int stat = getstatus(name_pid[name]);
			if(stat >= 3)
			{
				continue;
			} else
			{
				procinf inf = procL[name].front();
				exec_proc(inf);
				procL[name].pop();
			}
		}
	}
}
void Rerun()
{
	end_flag = true;
	pthread_join(delayed,NULL);
//	map<string,queue<procinf>> procL;
///	map<string,int> name_pid;
//	set<int> active;
//	set<int> pids;
//	set<string> launched;
///	set<string> delayed;
	procL.clear();
	name_pid.clear();
	active.clear();
	pids.clear();


}
void End_all()
{
	for(auto& pid : active)
	{
		int stat = getstatus(pid);
		if(stat >= 3)
			kill(pid,SIGKILL);
	}
}
int main(int arc,char* argv[])
{
	freopen(argv[1],"rt",stdin);
	int len = atoi(argv[2]);
	init_sem = sem_open("/semInit",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	read_sem = sem_open("/semRead",O_CREAT,S_IRWXG | S_IRWXO |S_IRWXU,1);
	return 0;
}
