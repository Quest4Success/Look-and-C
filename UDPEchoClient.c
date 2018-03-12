#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdbool.h>

void DieWithError(char *errorMessage); /* External error handling function */

typedef struct {

	enum {
		Login, Follow, Post, Request, Search, Delete, Unfollow, Logout
	}

	request_type; /* same size as an unsigned int */

	unsigned int UserID; /* unique client identifier */

	unsigned int LeaderID; /* unique client identifier */

	char message[140]; /* text message*/

} ClientMessage;

typedef struct {

	unsigned int ID; /* unique client identifier */

	char message[140]; /* text message*/

} ServerMessage;

int main(int argc, char *argv[]) {
	int sock; /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
	struct sockaddr_in fromAddr; /* Source address of echo */
	unsigned short echoServPort; /* Echo server port */
	unsigned int fromSize; /* In-out of address size for recvfrom() */
	char *servIP; /* IP address of server */
	bool logged = false; /* Log in status*/
	int id; /* Id to pass to Struct*/
	int choice; /* Choice from menu*/
	ClientMessage CMsg;
	ServerMessage SMsg;
	ClientMessage *Cpoint;
	ServerMessage *Spoint;
	Cpoint = &CMsg;
	Spoint = &SMsg;

	if ((argc < 3)) /* Test for correct number of arguments */
	{
		fprintf(stderr, "Usage: %s <Server IP> [<Echo Port>]\n\n", argv[0]);
		exit(1);
	}

	servIP = argv[1]; /* First arg: server IP address (dotted quad) */
	if(argc == 3)
        echoServPort = atoi(argv[2]);  /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */

	/* Create a datagram/UDP socket */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
	echoServAddr.sin_family = AF_INET; /* Internet addr family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
	echoServAddr.sin_port = htons(echoServPort); /* Server port */

	/* Send the string to the server */

	while (1) { // menu
		printf("\n1.Login\n");
		printf("2.Follow\n");
		printf("3.Post\n");
		printf("4.Request\n");
		printf("5.Search\n");
		printf("6.Delete\n");
		printf("7.Unfollow\n");
		printf("8.Logout\n");
		printf("0.Exit\n");
		scanf("%d", &choice);
		if (choice == 0) { // exit menu
			close(sock);
			exit(0);
		}
		if (logged == true || choice == 1) {
			switch (choice) {
			case 1: // log in
				logged = true;
				printf("Enter an userId\n");
				scanf("%d", &id);
				CMsg.request_type = 0;
				CMsg.UserID = id;
				break;
			case 2: // enter a leader to follow
				printf("Enter a Leader\n");
				scanf("%d", &id);
				CMsg.request_type = 1;
				CMsg.LeaderID = id;
				break;
			case 3: // post a message
				printf("Write what you please\n");
				scanf(" %140[^\n]%*c", CMsg.message);
				CMsg.request_type = 2;
				break;
			case 4: // print feed of followed leaders
				CMsg.request_type = 3;
				break;
			case 5: //find post using first word of post as key
				printf("Enter the first word of chosen post\n");
				scanf(" %s", CMsg.message);
				CMsg.request_type = 4;
				break;
			case 6: // delete post using first word of post as key
				printf("Enter a post to delete\n");
				scanf("%s", CMsg.message);
				printf("%s", CMsg.message);
				CMsg.request_type = 5;
				break;
			case 7: // unfollow leader
				printf("Enter a Leader\n");
				scanf("%d", &id);
				CMsg.LeaderID = id;
				CMsg.request_type = 6;
				break;
			case 8: // log out
				logged = false;
				CMsg.request_type = 7;
				break;
			}

		} else {
			strcpy(SMsg.message, "log in required"); // not logged in
			SMsg.ID = 0000;
		}
		/* Recv a response */
		if (sendto(sock, Cpoint, sizeof(CMsg), 0,
				(struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
			DieWithError(
					"sendto() sent a different number of bytes than expected");

		fromSize = sizeof(fromAddr);
		if ((recvfrom(sock, Spoint, sizeof(SMsg), 0,
				(struct sockaddr *) &fromAddr, &fromSize)) < 0)
			DieWithError("recvfrom() failed");

		if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
			fprintf(stderr, "Error: received a packet from unknown source.\n");
			exit(1);
		}

		while ((strcmp(SMsg.message, "All post from Leaders you follow ") != 0
				&& CMsg.request_type == 3)
				|| (strcmp(SMsg.message,
						"These are all the post matching the given tag") != 0
						&& CMsg.request_type == 4)) {
			printf("%s", SMsg.message);
			printf(" %d\n", SMsg.ID);
			memset(SMsg.message, 0, sizeof(SMsg.message) / sizeof(char)); // loop to get all post
			memset(CMsg.message, 0, sizeof(CMsg.message) / sizeof(char)); // if sending a stream

			if ((recvfrom(sock, Spoint, sizeof(SMsg), 0,
					(struct sockaddr *) &fromAddr, &fromSize)) < 0)
				DieWithError("recvfrom() failed");
		}
		printf("%s", SMsg.message);
		printf(" %d\n", SMsg.ID);					// print revcieved packet
		memset(SMsg.message, 0, sizeof(SMsg.message) / sizeof(char));
		memset(CMsg.message, 0, sizeof(CMsg.message) / sizeof(char));
	}
}
