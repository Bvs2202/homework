#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define PORT_SERV 7777

#define SIZE_BUFF 256
#define POOL_THREAD 5

int client_sockfd = -1;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *send_time(void *arg)
{
	char buff[SIZE_BUFF] = {0};
	time_t cur_time;
	struct tm *time_info;
	ssize_t byte_in;
	int thread_sockfd;

	while(1)
	{
		pthread_mutex_lock(&mutex);
		while (client_sockfd == -1)
		{
			pthread_cond_wait(&cond, &mutex);
		}
		thread_sockfd = client_sockfd;
		client_sockfd = -1;
		pthread_mutex_unlock(&mutex);

		while (1)
		{
			byte_in = recv(thread_sockfd, buff, sizeof(buff), 0);
			if (byte_in <= 0)
			{
				close(thread_sockfd);
				break;
			}
			time(&cur_time);
			time_info = localtime(&cur_time);
			strftime(buff, sizeof(buff), "%H:%M:%S", time_info);

			send(thread_sockfd, buff, strlen(buff), 0);
		}
	}
}

void create_pool_serv(void)
{
	pthread_t thread[POOL_THREAD];

	for (int i = 0; i < POOL_THREAD; i++)
	{
		if (pthread_create(&thread[i], NULL, send_time, NULL) != 0)
		{
			exit(EXIT_FAILURE);
		}
	}
}

int main()
{
	int sockfd;
	int new_sockfd;
	struct sockaddr_in serv_addr, client_addr;
	socklen_t len_addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket");
		return 1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT_SERV);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("bind");
		return 1;
	}

	if (listen(sockfd, 5) == -1)
	{
		perror("listen");
		return 1;
	}

	create_pool_serv();

	len_addr = sizeof(client_addr);

	while (1)
	{
		new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len_addr);
		{
			if (new_sockfd == -1)
			{
				break;
			}
		}

		pthread_mutex_lock(&mutex);
		client_sockfd = new_sockfd;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}

	close(sockfd);

	return 0;
}