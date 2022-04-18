
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

int main()
{
	char test[4001];
	int ret = 1;
	FILE* tmp;
	std::string file("/goinfre/file_0");
	while ((tmp = fopen(file.c_str(), "wx")) == NULL)
	{
		file[14]++;
		if (file[14] == '9')
			file.insert(14, "1");
	}
	if(tmp == NULL)
	{
		write(STDOUT_FILENO, "STO\n\n", 5);
		return (1);
	}
	int ret_wr;
	while(ret > 0)
	{
		ret = 0;
		ret_wr = 0;
		ret = read(STDIN_FILENO, test, 4000);
		if(ret <= 0)
			break;
		ret_wr = fwrite(test , sizeof(char), ret, tmp);
	}
	fclose(tmp);
	return(0);
}