
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <iomanip>


void	test(int sig_num)
{
	FILE* tmp1 = fopen("/goinfre/shackbei/upploads/error", "w");
	fprintf(tmp1, "sig = %d",  sig_num);
	fclose( tmp1);

}

void	displayTimestamp(void)
{
	std::time_t t = std::time(NULL);
  	std::tm *const pTInfo = std::localtime(&t);

	std::cerr << std::setfill('0') << "[" <<
	std::setw(4) << 1900 + pTInfo->tm_year <<
	std::setw(2) << pTInfo->tm_mon + 1 <<
	std::setw(2) << pTInfo->tm_mday << "_" <<
	std::setw(2) << pTInfo->tm_hour <<
	std::setw(2) << pTInfo->tm_min <<
	std::setw(2) << pTInfo->tm_sec << "] ";
}

int main()
{
	// signal(SIGINT, SIG_IGN);
	signal(1, test);
	// signal(2, test);
	signal(3, test);
	signal(4, test);
	signal(5, test);
	signal(6, test);
	signal(7, test);
	signal(8, test);
	signal(9, test);
	signal(10, test);
	signal(11, test);
	signal(12, test);
	signal(13, test);
	signal(14, test);
	signal(15, test);
	signal(16, test);
	signal(17, test);
	signal(18, test);
	signal(19, test);
	signal(20, test);
	signal(21, test);
	signal(22, test);
	signal(23, test);
	signal(24, test);
	signal(25, test);
	signal(26, test);
	signal(27, test);
	signal(28, test);
	signal(29, test);
	signal(30, test);
	signal(31, test);
	// write(STDOUT_FILENO, "test\n\n", 6);
	char test[4001];
	int ret = 1;
	FILE* tmp;
	std::string file("/goinfre/shackbei/upploads/pic0.jpg");
	while ((tmp = fopen(file.c_str(), "wx")) == NULL)
	{
		file[30]++;
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
		dprintf(2, "befor read\n");
		displayTimestamp();
		ret = read(STDIN_FILENO, test, 4000);
		dprintf(2, "after read\n");
		// fwrite("test0\n" , 1, 6,tmp);
		if(ret <= 0)
			break;
		ret_wr = fwrite(test , sizeof(char), ret, tmp);
		// fprintf(tmp, "\nret= %d ret_wr= %d\n", ret, ret_wr);
		// fwrite("test2\n" , 1, 6,tmp);
		dprintf(2, "after writing\n");
	}
	fclose(tmp);
	// write(STDOUT_FILENO, "end\n\n", 5);
	return(0);
}