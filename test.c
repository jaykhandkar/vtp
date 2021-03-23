#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>

int main()
{
	char a = 'p';
	char b = 'q';
	char *t = malloc(1);
	int fd = open("/dev/vtp0", O_RDONLY);

	*t = 'a';
	ioctl(fd, 0, &a);
	ioctl(fd, 0, &b);
	ioctl(fd, 0, t);
}
