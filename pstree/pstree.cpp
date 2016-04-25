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
	char temp[100];  // имя файла
	DIR* dir = opendir("/proc/"); // рабочая директория
	struct dirent* cur; // текущая открытая директория
	int pid = 0; // id процесса
	int ppid = 0; // id родительского процесса
	char un;
	while ( ( cur = readdir(dir) ) != NULL )
	{
	  if ( !isdigit ( cur->d_name[0] ) )
		  continue; // обработка директорий, не отвечающих за процессы
	  string path = "/proc/" + string(cur->d_name) + "/stat"; // путь к информационному файлу
	  FILE* file = fopen(path.c_str(),"r"); 
	  if ( file == NULL )
	  {
		  fclose(file);
		  continue;
	  }
	  fscanf(file, "%d %s %c %d", &pid, temp, &un, &ppid);
	  string pname = string(temp);
	  pname = pname.substr(1,pname.length()-2);// отбрасывание скобок у имени процесса
	  proc[ppid].push_back(make_pair(pid,pname));
	  fclose(file);
	}

	make_tree(0, "", -1);

return 0;
}
