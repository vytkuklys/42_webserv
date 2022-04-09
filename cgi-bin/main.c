#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *env[])
{
	execve("php-cgi", argv, env);
}