/* CMPE 207  Lab Assignment #2
/* Topic : Server Design
/* Author: Group 8
/* TCP Client */

#include <stdio.h>
#define _USE_BSD 1
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#define BUFSIZE 64

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif

extern int errno;

int errexit(const char *format,...);
int connectTCP(const int port,const char *host);
int connectsock(const int port,const char *host,const char *transport);

int main(int argc,char *argv[])
{
	char *host;
	char *filename;
	double result;
	int port;
	int sock,n;

	switch(argc)
	{
		case 1:
		host="localhost";
		break;

		case 2:
		host="localhost";
		break;

		case 4:
		host=argv[1];
		filename=argv[2];
		port=atoi(argv[3]);
		break;

		default:
		printf("Error in reading arguments\n");
		exit(1);
	}

	/* Establishes connectivity with the server by choosing TCP protocol. Returns a positive Integer on connecting. */

	sock=connectTCP(port,host);

	int  f;
	ssize_t sent_bytes,rcvd_bytes;
	char recv_str[20000];

	/* Sends the data to the server */
	printf("sending file : %s\n",filename);
	if((sent_bytes = send(sock,filename,20000,0))<0)
	{
   		printf("send error\n");
		close(sock);
		exit(0);
	}

	int create = 0;
	int flag=0;

	/* Receives data from the Server */
	while((rcvd_bytes = recv(sock,recv_str,20000,0))>0)
	{
    	/* Create a file or write to the file if it already exists. */
		if(create == 0)
		{
			printf("creating file\n");
			if((f = open(filename, O_WRONLY|O_CREAT, 0644))<0)
   				printf("Error creating file\n");
		}
		create = 1;
		printf("Writing to file\n");
		printf("%s\n",recv_str);

		/* Write to the file, the data received */
		if(write(f,recv_str,rcvd_bytes)<0)
		{
     			printf("error in writing to file");
		}
		flag=1;
		close(f);
		close(sock);
	}
	if(flag==0 && rcvd_bytes<=0)
	{
		printf("File not found\n");
	}

	close(sock);
	printf("Client Disconnected\n");
}

/* connectsock - allocate & connect a socket using TCP or UDP */

int connectTCP(const int port,const char *host)
{
	return connectsock(port,host,"tcp");
}

/*
* Arguments:
* host - name of host to which connection is desired
* port - desired port number
* transport - name of transport protocol to use ("tcp" or "udp")
*/

int connectsock(const int port,const char *host,const char *transport)
{
	struct sockaddr_in sin;int sock,type;

	memset(&sin,0,sizeof(sin));
	// Family Name
	sin.sin_family=AF_INET;
	// Port Number
	sin.sin_port=htons(port);
	inet_pton(AF_INET,host,&(sin.sin_addr));

	/* Use protocol to choose a socket type */

	if(strcmp(transport,"udp")==0)
	type=SOCK_DGRAM;
	else
	type=SOCK_STREAM;

 	/* Allocate a socket */

	sock=socket(AF_INET,type,0);

	if(sock<0)
		errexit("Error in creating socket : %s\n",strerror(errno));

	/* Connect the socket */

	if((connect(sock,(struct sockaddr *) &sin,sizeof(sin)))<0)
		errexit("Error in connection %s.%s: %s\n",host,port,strerror(errno));

	return sock;
}

int errexit(const char* format,...)
{
	va_list args;

	va_start(args,format);
	vfprintf(stderr,format,args);
	va_end(args);
	exit(1);
}

