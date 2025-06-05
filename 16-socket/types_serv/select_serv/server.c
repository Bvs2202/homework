#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/select.h>

#define PORT_SERV 7777

#define SIZE_BUFF 256

int main()
{
	char buff[SIZE_BUFF] = {0};
	int sockfd_tcp, sockfd_udp, max_fd;
	int new_sockfd_tcp;
	int ret;
	char *msg_tcp = "Привет, TCP клиент!";
	char *msg_udp = "Привет, UDP клиент!";
	fd_set readfds;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_udp_addr, client_tcp_addr;
	ssize_t byte_in;
	socklen_t len_addr;

	sockfd_tcp = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_tcp == -1)
	{
		printf("socket_tcp\n");
		return 1;
	}
	sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd_udp == -1)
	{
		printf("socket_udp\n");
		close(sockfd_tcp);
		return 1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_SERV);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	bind(sockfd_udp, (struct sockaddr *)&server_addr, sizeof(server_addr));
	bind(sockfd_tcp, (struct sockaddr *)&server_addr, sizeof(server_addr));

	listen(sockfd_tcp, 5);

	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(sockfd_tcp, &readfds);
		FD_SET(sockfd_udp, &readfds);
		if (sockfd_tcp > sockfd_udp)
		{
			max_fd = sockfd_tcp + 1;
		}
		else
		{
			max_fd = sockfd_udp + 1;
		}

		ret = select(max_fd, &readfds, NULL, NULL, NULL);

		if (ret == -1)
		{
			printf("select\n");
			close(sockfd_tcp);
			close(sockfd_udp);
			return 1;
		}
		else if (FD_ISSET(sockfd_tcp, &readfds))
		{
			memset(buff, 0, sizeof(buff));
			printf("\nПодключился TCP клиент.\n");
			len_addr = sizeof(client_tcp_addr);
			new_sockfd_tcp = accept(sockfd_tcp, (struct sockaddr *)&client_tcp_addr, &len_addr);

			recv(new_sockfd_tcp, buff, sizeof(buff), 0);
			printf("Сообщение от клиента: %s\n\n", buff);

			send(new_sockfd_tcp, msg_tcp, strlen(msg_tcp), 0);

			close(new_sockfd_tcp);
		}
		else if (FD_ISSET(sockfd_udp, &readfds))
		{
			memset(buff, 0, sizeof(buff));
			printf("\nПодключился UDP клиент.\n");

			len_addr = sizeof(client_udp_addr);
			recvfrom(sockfd_udp, buff, sizeof(buff), 0, (struct sockaddr *)&client_udp_addr, &len_addr);
			printf("Сообщение от клиента: %s\n\n", buff);

			sendto(sockfd_udp, msg_udp, strlen(msg_udp), 0, (struct sockaddr *)&client_udp_addr, sizeof(client_udp_addr));
		}
	}


	return 0;
}