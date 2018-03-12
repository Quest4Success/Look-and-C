#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdbool.h>

#define ECHOMAX 255     /* Longest string to echo */

void DieWithError(char *errorMessage); /* External error handling function */

typedef struct {

	enum {
		Login, Follow, Post, Request, Search, Delete, Unfollow, Logout
	}

	request_Type; /* same size as an unsigned int */

	unsigned int UserID; /* unique client identifier */

	unsigned int LeaderID; /* unique client identifier */

	char message[140]; /* text message*/

} ClientMessage;

typedef struct {

	unsigned int ID; /* unique client identifier */

	char message[140]; /* text message*/

} ServerMessage;

typedef struct {

	char post[50][140];			// 2d arrray for all post of user 50 max

	bool followers[50];			// array for follower

	bool following[50];			// array for who the user is following

	bool logged;				// know if user is logged on

	int postCount;				// keep track of how many post user has posted
} Account;

int main(int argc, char *argv[]) {
	int sock; /* Socket */
	struct sockaddr_in echoServAddr; /* Local address */
	struct sockaddr_in echoClntAddr; /* Client address */
	unsigned int cliAddrLen; /* Length of incoming message */
	unsigned short echoServPort; /* Server port */
	int recvMsgSize; /* Size of received message */
	int i;
	int j;
	ClientMessage CMsg;
	ServerMessage SMsg;
	ClientMessage *Cpoint;
	ServerMessage *Spoint;
	Cpoint = &CMsg;
	Spoint = &SMsg;
	char compare[25];

	Account Server[100];
	memset(Server, 0, sizeof(Server));				// zero out Structures
	if (argc != 2) /* Test for correct number of parameters */
	{
		fprintf(stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[0]);
		exit(1);
	}

	servIP = argv[1]; /* First arg: server IP address (dotted quad) */
	if(argc == 2)
        echoServPort = atoi(argv[1]);  /* Use given port, if any */
    else
        echoServPort = 7;  /* 7 is the well-known port for the echo service */

	/* Create socket for sending/receiving datagrams */
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	/* Construct local address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
	echoServAddr.sin_family = AF_INET; /* Internet address family */
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
	echoServAddr.sin_port = htons(echoServPort); /* Local port */

	/* Bind to the local address */
	if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		DieWithError("bind() failed");

	for (;;) /* Run forever */
	{
		/* Set the size of the in-out parameter */
		cliAddrLen = sizeof(echoClntAddr);

		/* Block until receive message from a client */
		if (recvfrom(sock, Cpoint, sizeof(CMsg), 0,
				(struct sockaddr *) &echoClntAddr, &cliAddrLen) < 0)
			DieWithError("recvfrom() failed");

		if (Server[CMsg.UserID].logged == true || CMsg.request_Type == 0) {
			switch (CMsg.request_Type) {
			case 0: // log in
				Server[CMsg.UserID].logged = true;
				SMsg.ID = CMsg.UserID;
				strcpy(SMsg.message, "Logged on as user number: ");
				break;
			case 1: // follow leader
				Server[CMsg.UserID].following[CMsg.LeaderID] = true;
				Server[CMsg.LeaderID].followers[CMsg.UserID] = true;
				SMsg.ID = CMsg.LeaderID;
				strcpy(SMsg.message, "Followed user: ");
				break;
			case 2: // post message
				strcpy(Server[CMsg.UserID].post[Server[CMsg.UserID].postCount],
						CMsg.message);
				Server[CMsg.UserID].postCount++;

				memset(&SMsg, 0, sizeof(ServerMessage));
				SMsg.ID = CMsg.UserID;
				strcpy(SMsg.message, "Message posted by");
				break;
			case 3: // all post from leaders

				for (i = 0; i < (sizeof(Server) / sizeof(Account)); i++) // check every user housed in server
						{
					// if leader has any post and user is following the leader then loop continues
					if (Server[CMsg.UserID].following[i] == true
							&& Server[i].postCount >= 0) {
						for (j = 0; j < Server[i].postCount; j++) {
							memset(&SMsg, 0,
									(sizeof(SMsg) / sizeof(ServerMessage)));
							strcpy(SMsg.message, Server[i].post[j]); //copy post to Server message
							SMsg.ID = i; // copy user who posted message
							if (sendto(sock, Spoint, sizeof(SMsg), 0,
									(struct sockaddr *) &echoClntAddr,
									sizeof(echoClntAddr)) < 0)
								DieWithError(
										"sendto() sent a different number of bytes than expected");
						}
					}
				}
				memset(&SMsg, 0, sizeof(ServerMessage));
				strcpy(SMsg.message, "All post from Leaders you follow ");
				SMsg.ID = CMsg.UserID;

				break;
			case 4:	 // all post matching given tag

				for (i = 0; i < (sizeof(Server) / sizeof(Account)); i++)// check every user housed in server
						{
					if (Server[i].postCount >= 0) {
						for (j = 0; j <= Server[i].postCount; j++) {// check all post of current user

							memset(&compare, 0, sizeof(compare)); // zero out compar array
							sscanf(Server[i].post[j], " %s", compare); // check beginning of current post

							if (strcmp(compare, CMsg.message) == 0) { // if beginning string matches tag the move 
								SMsg.ID = 1;		// onto sending it to user
								memset(&SMsg, 0,
										(sizeof(SMsg) / sizeof(ServerMessage))); // zero out server message
								strcpy(SMsg.message, Server[i].post[j]); // copy post to Server to be sent to User
								SMsg.ID = i;  // Tell user who posted post

								if (sendto(sock, Spoint, sizeof(SMsg),
										0, // send post
										(struct sockaddr *) &echoClntAddr,
										sizeof(echoClntAddr)) < 0)
									DieWithError(
											"sendto() sent a different number of bytes than expected");
							}
						}
					}
				}
				memset(&SMsg, 0, (sizeof(SMsg) / sizeof(ServerMessage)));
				strcpy(SMsg.message, "These are all the post matching the given tag");
				SMsg.ID = CMsg.UserID;
				break;

			case 5:
				// send message of user never posted anything
				if (Server[CMsg.UserID].postCount <= 0) {
					strcpy(SMsg.message, " No Post to delete");
					SMsg.ID = CMsg.UserID;
				} else {
					for (i = 0; i < Server[CMsg.UserID].postCount; i++) // check all post up until postcount

							{
						memset(&compare, 0, (sizeof(compare) / sizeof(char))); // zero out comparing array of chars
						sscanf(Server[CMsg.UserID].post[i], " %s", compare); //

						if (strcmp(compare, CMsg.message) == 0) //if given string matches beginning
						 {											//of post then the post is marked for deletion

							for (j = i; j <= Server[CMsg.UserID].postCount - 1;
									j++) // check each loop
									{
								memset(Server[CMsg.UserID].post[j], 0,
										sizeof(Server[CMsg.UserID].post[j])
												/ sizeof(char)); //zero out spot
								strcpy(Server[CMsg.UserID].post[j],
										Server[CMsg.UserID].post[j + 1]); // copy spot after
							}
						
					}
				}
			}
			memset(&SMsg, 0, (sizeof(SMsg) / sizeof(ServerMessage))); // zero out structure
						strcpy(SMsg.message, " Post Deleted");
						SMsg.ID = CMsg.UserID;
			Server[CMsg.UserID].postCount--; // decrement post array
			break;
			case 6:
			// follow leader
			memset(&SMsg, 0, sizeof(ServerMessage));
			if (Server[CMsg.UserID].following[CMsg.LeaderID] == true) {
				Server[CMsg.UserID].following[CMsg.LeaderID] = false;
				Server[CMsg.LeaderID].followers[CMsg.UserID] = false;
				SMsg.ID = CMsg.LeaderID;
				strcpy(SMsg.message, "Unfollowed Leader:");
			} else { // if leader wasnt being followed then doesnt follow
				SMsg.ID = CMsg.LeaderID;
				strcpy(SMsg.message, "Leader was never being followed");
			}

			break;
			case 7:
			//set bool val logged to false
			Server[CMsg.UserID].logged = false;
			SMsg.ID = CMsg.UserID;
			strcpy(SMsg.message, "Logged out as user: ");
			break;
		}

		printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

		//  Send received datagram back to the client  

		if (sendto(sock, Spoint, sizeof(SMsg), 0,
				(struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) < 0)
			DieWithError(
					"sendto() sent a different number of bytes than expected");
	}
else{ // incase client is not logged on within server
                SMsg.ID = 0000;
                strcpy(SMsg.message,"Log in needed");
                
                if (sendto(sock, Spoint, sizeof(SMsg), 0,
                    (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) < 0)
                DieWithError("sendto() sent a different number of bytes than expected");
                }
    	memset(&SMsg, 0, sizeof(ServerMessage)); // zero out structure for new input
	memset(&CMsg, 0, sizeof(ClientMessage));
}
}
