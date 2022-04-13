
#include <stdio.h>
#include <unistd.h>
#include <string>

int main()
{
	// write(STDOUT_FILENO, "test\n\n", 6);
	char test[4001];
	int ret = 1;
	FILE* tmp;
	std::string file("./pic0");
	while ((tmp = fopen(file.c_str(), "wx")) == NULL)
	{
		file[5]++;
	}
	if(tmp == NULL)
	{
		write(STDOUT_FILENO, "STO\n\n", 5);
		return (1);
	}
	while(ret > 0)
	{
		ret = read(STDIN_FILENO, test, 4000);
		fwrite(test , 1, ret, tmp);
	}
	fclose(tmp);
	// write(STDOUT_FILENO, "end\n\n", 5);
	return(0);
}