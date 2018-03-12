#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdbool.h>

typedef struct
{

    enum {c2s_login, c2s_who, c2s_lookup, c2s_logout }
    request_type;                               /* same size as an unsigned int */

    unsigned int player_id;                       /* initiating player identifier */

    unsigned short tcp_port;                     /* listening port*/

    unsigned int ip_address;                    /*ip_address */

} client_2_server_message;                   /* an unsigned int is 32 bits = 4 bytes */

typedef struct
{

    enum {s_ok, s_who, s_lookup,s_deny }
    response_type;        /* same size as an unsigned int */

    bool avail_players[10];                         /* list of player identifiers */

    unsigned int requested_id;                 /* requested client id */

    unsigned short tcp_port;                     /* requested port */

    unsigned int ip_address;                     /* requested IP address */

} server_message;

void DieWithError(char *errorMessage); /* External error handling function */


int main(int argc, char *argv[])
{
    int sock; /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen; /* Length of incoming message */
    unsigned short echoServPort; /* Server port */

    bool avail[11];
    memset(avail,false,sizeof(avail));
    int i; //loop controls
    int j;
    server_message SMsg[11];//server for all clients
    client_2_server_message CMsg;//server to client message

    memset(&SMsg, 0, sizeof(SMsg));				// zero out Structures
    memset(&CMsg, 0, sizeof(CMsg));

    if (argc != 2) /* Test for correct number of parameters */
    {
        fprintf(stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }
    echoServPort = atoi(argv[1]); /* First arg:  local port */

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
        if (recvfrom(sock, &CMsg, sizeof(CMsg), 0,
                     (struct sockaddr *) &echoClntAddr, &cliAddrLen) < 0)
            DieWithError("recvfrom() failed");

            switch (CMsg.request_type)  
            {
            case 0:// log in
                if(avail[CMsg.player_id] == true) //if user tries to log on with a ID already logged in
                {
                    SMsg[CMsg.player_id].requested_id = 4; //logged already status
                    SMsg[CMsg.player_id].requested_id  = CMsg.player_id;
                    printf("User Number %d already taken",CMsg.player_id); 
                }
                else //gather all information about user
                {
                    avail[CMsg.player_id] = true;
                    SMsg[CMsg.player_id].response_type = 0;
                    SMsg[CMsg.player_id].requested_id  = CMsg.player_id;
                    SMsg[CMsg.player_id].ip_address  = atoi(inet_ntoa(echoClntAddr.sin_addr));
                    SMsg[CMsg.player_id].tcp_port = CMsg.tcp_port;
                    printf("Logged client at %s in as ID: %d \n", inet_ntoa(echoClntAddr.sin_addr),CMsg.player_id);
                }
                break;
            case 1:// who query
                memset(&SMsg[CMsg.player_id].avail_players,0,sizeof(SMsg[CMsg.player_id].avail_players));
                for(i =1; i<sizeof(avail); i++) //copy online status from outside array of online clients
                {
                    SMsg[CMsg.player_id].avail_players[i] = avail[i];
                }
                SMsg[CMsg.player_id].response_type = 1;
                printf("Sending currently logged users to client %d\n",SMsg[CMsg.player_id].requested_id );
                break;
            case 2:// lookup TCP port of a requested client
                SMsg[CMsg.player_id].response_type = 2;
                printf("Getting address for client %d\n", SMsg[CMsg.player_id].requested_id);
                break;
            case 3:// Logout
                avail[CMsg.player_id] = false;
                printf("Logged client %s out as ID: %d \n", inet_ntoa(echoClntAddr.sin_addr),CMsg.player_id);
                break;
            }
        //  Send received datagram back to the client
        if (sendto(sock, &SMsg[CMsg.player_id], sizeof(SMsg[CMsg.player_id]), 0,
                   (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) < 0)
            DieWithError("sendto() sent a different number of bytes than expected");

        memset(&CMsg,0,sizeof(CMsg)); //zero out server to client message structure

    }
}
