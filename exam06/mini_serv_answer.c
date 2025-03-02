#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/ip.h>

// echo "2^16 | bc" (using basic calculator in terminal)
// 65536

int		count = 0, max_fd = 0;
//to store identifiers for clients connected to the server
int		ids[65536];
char	*msgs[65536];

//rfds is used to monitor file descriptors for reading data from clients

//wfds could be used to monitor file descriptors for writing data, but it's not utilized in this code.

//afds is used to keep track of all active file descriptors, 
//which includes both the listening socket (sockfd) and any connected client sockets.
fd_set	rfds, wfds, afds;
char	buf_read[1001], buf_write[42];

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

void	fatal_error() 
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

//to inform all other clients that a new client has arrived
void	notify_other(int author, char *str) 
{
	for (int fd = 0; fd <= max_fd; fd++) 
	{
		// To determine if a client socket is ready to receive a message.
		// and if it's not the same as the author (the client that triggered the event)
		if (FD_ISSET(fd, &wfds) && fd != author)
			send(fd, str, strlen(str), 0);
	}
}


void	register_client(int fd) 
{
	if (fd > max_fd)
		max_fd = fd;
	ids[fd] = count++;
	msgs[fd] = NULL;
	FD_SET(fd, &afds);
	// Format and store values("server: client %d just arrived\n") into the buf_write
	sprintf(buf_write, "server: client %d just arrived\n", ids[fd]);
	notify_other(fd, buf_write);
}

void	remove_client(int fd) 
{
	sprintf(buf_write, "server: client %d just left\n", ids[fd]);
	notify_other(fd, buf_write);
	free(msgs[fd]);
	FD_CLR(fd, &afds);
	close(fd);
}

void	send_msg(int fd) 
{
	char *msg;

	// Continue extracting messages until there are no more messages
	while (extract_message(&(msgs[fd]), &msg)) 
	{
		// Format the message with client identifier
		sprintf(buf_write, "client %d: ", ids[fd]);
		// Notify other clients about the sender and the message
		notify_other(fd, buf_write); // Notify about the sender
		notify_other(fd, msg); // Notify about the message
		// Free the memory allocated for the extracted message
		free(msg);
	}
}

int		create_socket() 
{
	max_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (max_fd < 0)
		fatal_error();
	//add the socket file descriptor to the afds set
	FD_SET(max_fd, &afds);
	return max_fd;
}

int		main(int ac, char **av) 
{
	if (ac != 2) 
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	//Initializing afds before adding any file descriptors ensures 
	//that it is properly initialized and ready for use. 
	FD_ZERO(&afds);
	int sockfd = create_socket();

	struct sockaddr_in servaddr;
	// socket create and verification 
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT 
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(atoi(av[1])); // replace 8080

	// Binding newly created socket to given IP and verification 
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)))
		fatal_error();
	if (listen(sockfd, SOMAXCONN)) // the main uses 10, SOMAXCONN is 180 on my machine
		fatal_error();

	while (1) 
	{
		rfds = wfds = afds;
		//to monitor multiple file descriptors for readiness to perform I/O operations
		if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)
			fatal_error();

		for (int fd = 0; fd <= max_fd; fd++) 
		{
			//to test if a file descriptor is still present in a set
			if (!FD_ISSET(fd, &rfds))
				continue;

			if (fd == sockfd) 
			{
				socklen_t addr_len = sizeof(servaddr);
				int client_fd = accept(sockfd, (struct sockaddr *)&servaddr, &addr_len);
				if (client_fd >= 0) {
					register_client(client_fd);
					break ;
				}
			}
			else 
			{
				//If no data is available or an error occurs, recv() returns -1
				int read_bytes = recv(fd, buf_read, 1000, 0);
				if (read_bytes <= 0) {
					remove_client(fd);
					break ;
				}
				buf_read[read_bytes] = '\0';
				msgs[fd] = str_join(msgs[fd], buf_read);
				send_msg(fd);
			}
		}
	}
	return 0;
}

