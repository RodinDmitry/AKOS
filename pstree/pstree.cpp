
#include <iostream>
#include <cstdio>
#include <vector>
#include <dirent.h>
#include <map>
#include <algorithm>
#include <cstdlib>

using namespace std;
map<int, vector<pair<int, string> > > proc; // структура, хранящая процессы
// строит дерево процессов, рекурсивно вызываясь от всех процессов в списке
// начальный процесс имеет индекс 0
void make_tree(int vertex, string name, int tabs = 0)
{
	if (name != "")
	{
		for (int i = 0; i < tabs; i++)
			cout << "____";
		cout << name << endl;
	}
	vector<pair<int, string> > vecproc = proc[vertex];
	for (int i = 0; i < (int)vecproc.size(); i++)

		make_tree(vecproc[i].first, vecproc[i].second, tabs + 1);
}


int main()
{
	char temp[100];
	DIR* dir = opendir("/proc/");
	struct dirent* cur;
	int pid = 0;
	int ppid = 0;
	char un;
	int minroot = 10000000; // INF
	while ( ( cur = readdir(dir) ) != NULL )
	{
	if ( !isdigit ( cur->d_name[0] ) )
			continue;
	  string path = "/proc/" + string(cur->d_name) + "/stat";
	  FILE* file = fopen(path.c_str(),"r"); 
	  if ( file == NULL )
	  {
		  fclose(file);
		  continue;
	  }
	  if(ppid < minroot)
		 minroot = ppid;
	  fscanf(file, "%d %s %c %d", &pid, temp, &un, &ppid);
	  string pname = string(temp);
	  pname = pname.substr(1,pname.length()-2);// отбрасывание скобок у имени процесса
	  proc[ppid].push_back(make_pair(pid,pname));
	  fclose(file);
	}

	make_tree(minroot, "", -1);

return 0;
}
