
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <fcntl.h>
#include <fstream>


void	test(int sig_num)
{
	FILE* tmp1 = fopen("/goinfre/shackbei/upploads/error", "w");
	fprintf(tmp1, "sig = %d",  sig_num);
	fclose( tmp1);

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
	char buf[1];
	int ret = 1;
	int tmp;
	std::fstream file;
	file.open("/dev/random", std::ios::in);
	file.read(buf, 1);
	tmp = open((std::string("/goinfre/shackbei/upploads/") + std::string(buf, 1)).c_str(), O_WRONLY | O_CREAT);
	if(tmp == -1)
	{
		write(STDERR_FILENO, "STO\n\n", 5);
		return (1);
	}
	// write(tmp, "test0\n", 6);
	int ret_wr;
	while(ret > 0)
	{
		ret = 0;
		ret_wr = 0;
		ret = read(STDIN_FILENO, test, 4000);
		write(tmp, "test0\n", 6);
		ret_wr = write(tmp , test, ret);
		dprintf(tmp, "ret= %d ret_wr= %d\n", ret, ret_wr);
		write(tmp, "test1\n", 6);

	}
	close(tmp);
	// write(STDOUT_FILENO, "end\n\n", 5);
	return(0);
}