#include<unistd.h>
#include<iostream>
#include<stdio.h>
#include<string>
#include<vector>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;

void  Run_New(const char* name,vector<string> arg)
{
	int k = 0;
    while( k < arg.size() && arg[k] != ">" )
    {
    	k++;
    }
    char ** argv = new char * [k+1];
    argv[0] = const_cast<char*>(name);
    for(int i = 1;i <= k ;++i)
    {
    	argv[i] = const_cast<char*>(arg[i-1].c_str());
    }
    if(k < arg.size())
    {
    	if(k + 1 < arg.size())
    	{
    		freopen(arg[k+1].c_str(),"wt",stdout);
    	} else
    	{
    		printf("Error: output file name not found\n");
    		return;
    	}
    }
    execv(name,argv);
  
}
vector<string> Pars(string s)
{
	int last = 1;
	vector<string> ans;
	for(int i = 1;i < s.size();++i )
	{
		if(s[i] == ' ' || i + 1 == s.size())
		{
			if(s[i-1] != ' ')
				if(i + 1 != s.size() || s[i] == ' ')
				{
				ans.push_back(s.substr(last,i - last));
				last = i + 1;
				} else
				{
					ans.push_back(s.substr(last,i - last + 1));
				}
		}
	}
	return ans;
}
int main(int argc, char * const argv[])
{
	bool flag = true;
	string name;
	cin >> name;
	if (name == ":q")
		{
		    return 0;
		}
       int marker = fork();
	       if( marker == 0)
	       {
				vector<string>  arg;
				string param;
				std::getline(cin,param);
				arg = Pars(param);
				Run_New(name.c_str(),arg);
			}
	         else
			{
			   int status;
			   while(waitpid(-1,&status,0) != -1);
			   execve(argv[0],argv,NULL);
			}
	return 0;
}


