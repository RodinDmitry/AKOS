#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<unistd.h>
#include<iostream>
#include<stdio.h>
#include<string>
#include<vector>
#include<stdlib.h>
#include<fcntl.h>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;

struct obj
{
	string proc;
	vector<string> param;
	obj(string _proc): proc(_proc){};
};
char ** Pars_argv(obj arg)
{
	char** ans = new char*[arg.param.size() + 1];
	ans[0] = const_cast<char *>(arg.proc.c_str());
	for(int i = 1;i <= arg.param.size();++i)
	{
		ans[i] = const_cast<char *>(arg.param[i-1].c_str());
	}
	return ans;
}
vector<obj> Pars(string s,string& output,bool& IsOut)
{
	string temp;
	int i = 0;
	int j = -1;
	vector<obj> ans;
	while(i < s.size())
	{
		if(i < s.size() &&  s[i] == ' ')
		{
			++ i;
			continue;
		}
		temp.clear();
		if(s[i] == '|' || i == 0)
		{
			if(s[i] == '|')
				++i;
			while(i < s.size() && s[i] == ' ')
				++i;
			while(i < s.size() && s[i] != ' ')
			{
				temp += s[i];
				++i;
			}
			ans.push_back(obj(temp));
			++j;
			continue;
		}
		if(s[i] == '>')
		{
			++i;
			while(s[i] == ' ')
				++i;
			temp.clear();
			while(i < s.size() && s[i] != ' ')
			{
				temp += s[i];
				++i;
			}
			IsOut = true;
			output = temp;
			continue;
		}
		temp.clear();
		while(i < s.size() && s[i] == ' ')
			++i;
		while(i < s.size() && s[i] != ' ')
		{
			temp += s[i];
			++i;
		}
		ans[j].param.push_back(temp);
	}
	return ans;
}
int Run_New(obj pr)
{
	char ** argv = Pars_argv(pr);
	execve(argv[0],argv,0);
}
void exec_seq(vector<obj>& vcom,string& output,bool IsOut,int outid)
{
	 int tpipe[2];
	 int fdsk = outid;
	 if(IsOut)
	 {
		 FILE* file = fopen(output.c_str(),"w");
		 fdsk =	fileno(file);
	 }
	 int status;
	 for(int i  = 0 ; i < vcom.size();++i)
	  {
		  pipe(tpipe);

		  status = fork();
		  if(status != 0)
		  {
			  int outdsk = fileno(stdout);
			  close(tpipe[0]);
			  if(IsOut && i == vcom.size() - 1)
			  {
				  dup2(fdsk,outdsk);
			  } else
			  {
				  dup2(tpipe[1],outdsk);
			  }
			  Run_New(vcom[i]);
		  } else
		  {
			  close(tpipe[1]);
			  int indsk = fileno(stdin);
			  dup2(tpipe[0],indsk);
		  }
	  }
	 exit(0);
}

int main(int argc, char * const argv[])
{
  string com;
  string output;
  FILE * file = fopen("output.txt","w");
  std::getline(cin,com);
  bool IsOut = false;
  vector<obj> vcom;
  if( com.size() < 1)
  {
	  printf("Empty command\n");
	  execve(argv[0],argv,NULL);
  }
  if(com.size() > 1 && com[0] == ':' && com[1] == 'q' )
  {
	  return 0;
  } else
  {
	  output.clear();
	  vcom = Pars(com,output,IsOut);
  }
  int outid = fileno(file);
  int status = fork();
  if(status == 0)
  {
	  exec_seq(vcom,output,IsOut,outid);
  } else
  {
	  int res = 0;
	  waitpid(status,&res,0);
  }
  //exec_seq(vcom,output,IsOut);
  execve(argv[0],argv,0);
  return 0;
}









