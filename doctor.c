#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "doctor.h"
#include "service_desk.h"
#include "utils.h"

int	main(int argc, char *argv[])
{
	Doctor	me = {"", "", getpid(), false};
	char		dfifo[20] = "";
	char    message[255] = "";
	int			fd;

	if (serviceDeskIsRunning(0) == false)
	{
		fprintf(stderr, "The service desk isn't running\n");
		exit(0);
	}
	if (argc < 3)
	{
		fprintf(stderr, "%s <name> <speciality>\n", argv[0]);
		exit(0);
	}
	sprintf(dfifo, DFIFO, me.pid);
	if (mkfifo(dfifo, 0600) == -1)
	{
		fprintf(stderr, "Error occured while trying to make FIFO\n");
		exit(0);
	}
	if (access(SFIFO, F_OK) != 0)
	{
		fprintf(stderr, "Error, service desk FIFO doesn't exist\n");
		unlink(dfifo);
		exit(0);
	}
	strncpy(me.name, argv[1], sizeof(me.name));
	strncpy(me.speciality, argv[2], sizeof(me.speciality));
	if ((fd = open(SFIFO, O_WRONLY)) == -1)
	{
		fprintf(stderr, "Couldn't open named pipe file\n");
		unlink(dfifo);
		exit(0);
	}
	if (write(fd, "D", 1) == -1)
	{
		fprintf(stderr, "Couldn't write to named pipe\n");
		close(fd);
		unlink(dfifo);
		exit(0);
	}
	if (write(fd, &me, sizeof(Doctor)) == -1)
	{
		fprintf(stderr, "Couldn't write to named pipe\n");
		close(fd);
		unlink(dfifo);
		exit(0);
	}
	while (strcmp(message, "exit") != 0)
	{
		printf("command: ");
		scanf("%255s", message);
	}
	close(fd);
	return (0);
}
