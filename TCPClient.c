#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <sys/mman.h>   /* for mmap() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <stdbool.h>    /* for  boolean*/


typedef struct
{

    enum {c2s_login, c2s_who, c2s_lookup, c2s_logout }
    request_type;                               /* same size as an unsigned int */

    unsigned int player_id;                       /* initiating player identifier */

    unsigned short tcp_port;                     /* listening port*/

} client_2_server_message;                    /* an unsigned int is 32 bits = 4 bytes */

typedef struct
{

    enum {s_ok, s_who, s_lookup,s_deny}
    response_type;        /* same size as an unsigned int */

    bool avail_players[10];                         /* list of player identifiers */

    unsigned int requested_id;                 /* requested client id */

    unsigned short tcp_port;                     /* requested port */

    unsigned int ip_address;                     /* requested IP address */

} server_message;

typedef struct
{

    enum {c2c_play, c2c_accept, c2c_decline, c2c_move}
    request_type;     /* same size as an unsigned int */

    unsigned int player_id;                       /* initiating player identifier */

    unsigned int mov_row;                     /* player move – 0, 1, 2 */

    unsigned int mov_col;                         /* player move – 0, 1, 2 */

} client_2_client_message;                              /* an unsigned int is 32 bits = 4 bytes */

void DieWithError(char *errorMessage); /* External error handling function */


int main(int argc, char *argv[])
{
    int sockServ,hold,player_id,i,j,*win,
        col,row,processID,msg;  /* Socket descriptor */
    struct sockaddr_in servAddr; /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    struct sockaddr_in playerAddr;  /*playerAddr and Struct for listen*/
    unsigned short serverPort,playerPort;     /* Echo server port */
    unsigned int fromSize;      /* In-out of address size for recvfrom() */
    bool logged = false;            /* Log in status*/
    char *servIP;                    /* Server IP address (dotted quad) */
    char g[3][3],*gameBoard;            /* Game board*/
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv()
                                        and total bytes read */

    client_2_server_message c_2_server; /*client to server message structure*/
    client_2_client_message *player,p; /*client to client structure for mmap and for send/recieving*/
    server_message server;              /*server to client message structure


    memset(&server, 0, sizeof(server));         /*zero out structures*/
    memset(&c_2_server, 0, sizeof(c_2_server));
    memset(&p, 0, sizeof(p));

    player = (client_2_client_message *)mmap(NULL, sizeof(client_2_client_message), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON,-1, 0);   /*client to client mapped structure*/
    gameBoard = mmap(NULL,(sizeof(g)),PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,-1,0);  /*mapped tic-tac-toe area*/
    win = (int *)mmap(NULL,(sizeof(int)),PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON,-1,0);       /*mapped win flag*/

    *win = 0;
    for(i=0; i<9; i++)
    {
        gameBoard[i] = ' '; /* set each square to empty space*/
    }
    if ((argc < 4)) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server IP> [<Server Port>] <Player Port>\n",
                argv[0]);
        exit(1);
    }

    servIP = argv[1]; /* First arg: server IP address (dotted quad) */
    serverPort = atoi(argv[2]);  /* Use given port, if any */
    c_2_server.tcp_port =atoi(argv[3]); /*give player to player port to server struct*/
    playerPort = atoi(argv[3]);     /*set player to player connection port*/

    /* Create a datagram/UDP socket */
    if ((sockServ = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&servAddr, 0, sizeof(servAddr));     /* Zero out structure */
    servAddr.sin_family      = AF_INET;             /* Internet address family */
    servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    servAddr.sin_port        = htons(serverPort); /* Server port */

   
    do
    {   //login
        printf("Enter a User number between 1 and 10 to login\n");
        scanf("%d",&player_id);         //player login ID
        c_2_server.player_id = player_id;   //give login to server struct
        c_2_server.request_type = 0;    // login in status
        /* Send the Struct to the server */
        if ((i=sendto(sockServ, &c_2_server, sizeof(c_2_server), 0,
                      (struct sockaddr *) &servAddr, sizeof(servAddr))) < 0)
            DieWithError("sendto() sent a different number of bytes than expected");

        fromSize = sizeof(fromAddr);
        // receive login status
        if ((i =(recvfrom(sockServ, &server, sizeof(server), 0,
                          (struct sockaddr *) &fromAddr, &fromSize))) < 0)
            DieWithError("recvfrom() failed");

        if(server.response_type==4) // login already taken
            printf("User number %d already taken\n",server.requested_id);
    }
    while(server.response_type==4); // go through procss if login is taken already

    printf("Logged in as player: %d\n", server.requested_id);

    if((processID = fork())<0)  // begin fork
    {
        DieWithError("Child Process unable to be created");
    }
    else if(processID==0)   // child process
    {
        int playerAddrLen,playerSock,accptSock;

        while(1)
        {
            /* Construct TCP listen socket */
            if ((playerSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                DieWithError("socket() failed");

            
            memset(&playerAddr, 0, sizeof(playerAddr));   /* Zero out structure */
            playerAddr.sin_family = AF_INET;                /* Internet address family */
            playerAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
            playerAddr.sin_port = htons(playerPort);      /* Local port */

            //bind address to port
            if (bind(playerSock, (struct sockaddr *) &playerAddr, sizeof(playerAddr)) < 0)
                DieWithError("bind() failed");

            // put socket into listen mode with max 5 pending connections
            if((listen(playerSock, 5)) < 0)
                DieWithError("listen() failed");

            memset(&p,0,sizeof(p));

            // recieve incoming connection
            if((accptSock=accept(playerSock, (struct sockaddr *) &playerAddr,&playerAddrLen)) < 0)
                DieWithError("accept() failed");
            while(1) // check for end of message stream
            {   
                if ((msg=recv(accptSock, &p, sizeof(p), 0)) < 0)
                    DieWithError("recv() failed");

                if(!msg)
                    break;
            }
            //copy recieved struture to mapped memory
            player->request_type = p.request_type;
            player->player_id = p.player_id;
            player->mov_col = p.mov_col;
            player->mov_row = p.mov_row;
           
            //menu for recieved message
            switch(player->request_type)
            {
            case 0: // let user know someone has challenged 
                printf("\nChallenge received from user:%d\nPress 4 to respond\n", player->player_id);
                break;
            case 1: // notify of accepted challenge issued
                printf("\nUser: %d  accepted play request\nYou are assigned O\nPress 4 to respond with a move\n", player->player_id);
                break;
            case 2:// notify if issue challenged was declined
                printf("\nUser declined challenge request\n");
                break;
            case 3: // update game

                gameBoard[((3*player->mov_row)+player->mov_col)]='X'; // opponent move input to Board
                i=0;
                while(i<9)  // check for any win
                {
                    if(gameBoard[i] == 'X' || gameBoard[i] == 'O')
                    {
                        if((i%3 == 0) &&
                                (gameBoard[i] == gameBoard[i+1] && gameBoard[i] == gameBoard[i+2])) //check for horizontal win
                        {
                            printf("\n%c's Won!\n",gameBoard[i]);
                            *win =1;
                            break;
                        }
                        else if((i<3) && (gameBoard[i] == gameBoard[i+3] && gameBoard[i] == gameBoard[i+6])) //check for vertical win
                        {
                            printf("\n%c's Won!\n",gameBoard[i]);
                            *win = 1;
                            break;
                        }
                        else if((i==0  && gameBoard[i] == gameBoard[i+4] && gameBoard[i] == gameBoard[i+8])||
                                (i==2 && gameBoard[i] == gameBoard[i+2] && gameBoard[i] == gameBoard[i+4])) //check for diagnol win
                        {
                            printf("\n%c's Won!\n",gameBoard[i]);
                            *win = 1;
                            break;
                        }
                    }
                    i++;
                }
				
				printf("\nPress 4 to respond with a move\n" //display board
                for(i=0; i<9; i++)
                {
                    if((i%3)==2)
                    {
                        printf("%c",gameBoard[i]);
                        printf("\n");
                    }
                    else
                        printf("%c",gameBoard[i]);
                    if((i%3)<2)
                        printf(" | ");
                }
                break;
            }
            close(accptSock);
            close(playerSock); // close streams after done
        }
    }
    else
    {
        int col = 0, row = 0,choice,playerSock,game_choice;

        while(1)
        {
            printf("\n1.Who's Online\n");
            printf("2.Challenge a Player\n");
            printf("3.Logout\n");
            scanf("%d", &choice);
            switch (choice)
            {
            case 1: // who
                c_2_server.request_type = 1;
                printf("\nLooking Who's Online\n\n");
                break;
            case 2: // lookup
                printf("\nEnter Player's ID\n");
                scanf("%d", &choice);
                c_2_server.player_id = choice;
                c_2_server.request_type = 2;
                player->request_type = 0;
                break;
            case 3: // Logout
                c_2_server.request_type = 3;
                //send logout flag to server
                if (sendto(sockServ, &c_2_server, sizeof(c_2_server), 0,
                           (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) 
                    DieWithError("sendto() sent a different number of bytes than expected");
                close(sockServ);
                close(playerSock); //close sockets
                kill(processID,9); // stop child process when logged out
                printf("Logout successful\n");
                exit(0);
                break;
            case 4:// Responses to challenges
                printf("\nChoose a response\n");
                printf("\n1.Accept Challenge\n");
                printf("2.Decline Challenge\n");
                printf("3.Move\n");
                scanf("%d",&game_choice);
                switch(game_choice)
                {
                case 1:// accept a challenge
                    player->request_type = 1;
                    c_2_server.player_id = player->player_id; 
                    c_2_server.request_type = 2;

                    printf("\nAccepted Challenge, wait for Player to make first move\n");
                    printf("You are assigned O's\n"); 
                    break;
                case 2:
                    player->request_type = 2; //decline dont send flag
                    printf("\nDeclined Challenge\n");
                    break;
                case 3:
                    player->request_type = 2; //decline/dont send flag
                    if(*win == 0) // check win flag, tripped upon fork's recv
                    {
                        do
                        {
                            printf("\nInput a row (0-2), followed by a col(0-2)\n"); // grid prompt
                            scanf("%d %d", &row, &col);
                            if(gameBoard[((3*row)+col)]!=' ') //check if something is in spot picked by user
                            {
                                printf("\nSpot is already occupied choose again\n");

                                for(i=0; i<9; i++) //display Board so User can re-pick
                                {
                                    if((i%3)==2)
                                    {
                                        printf("%c",gameBoard[i]);
                                        printf("\n");
                                    }
                                    else
                                        printf("%c",gameBoard[i]);
                                    if((i%3)<2)
                                        printf(" | ");
                                }
                            }
                        }
                        while((row > 3 && col > 3) || gameBoard[((3*row)+col)]!=' '); //loops again if picked a spot filled

                        player->mov_row = row;
                        player->mov_col = col;
                        player->request_type = 3;
                        player->player_id = player_id;

                        gameBoard[((3*player->mov_row)+player->mov_col)]='O'; //update Board
                        printf("\n");
                        i=0;
                        while(i<9) //check for win after Board Update
                        {
                            if(gameBoard[i] == 'X' || gameBoard[i] == 'O')
                            {
                                if((i%3 == 0) &&
                                        (gameBoard[i] == gameBoard[i+1] && gameBoard[i] == gameBoard[i+2]))
                                {
                                    *win =1;
                                    printf("\n%c's Won!\n",gameBoard[i]);
                                    break;
                                }
                                else if((i<3) && (gameBoard[i] == gameBoard[i+3] && gameBoard[i] == gameBoard[i+6]))
                                {
                                    *win = 1;
                                    printf("\n%c's Won!\n",gameBoard[i]);
                                    break;
                                }
                                else if((i==0  && gameBoard[i] == gameBoard[i+4] && gameBoard[i] == gameBoard[i+8])||
                                        (i==2 && gameBoard[i] == gameBoard[i+2] && gameBoard[i] == gameBoard[i+4]))
                                {
                                    *win = 1;
                                    printf("\n%c's Won!\n",gameBoard[i]);
                                    break;
                                }
                            }
                            i++;
                        }

                        for(i=0; i<9; i++) //Display Board after Updated/Chekced for win
                        {
                            if((i%3)==2)
                            {
                                printf("%c",gameBoard[i]);
                                printf("\n");
                            }
                            else
                                printf("%c",gameBoard[i]);
                            if((i%3)<2)
                                printf(" | ");
                        }
                        printf("\nUser: %d input moves row:%d , column:%d\n",player->player_id,player->mov_row,player->mov_col); // Tell User their move
                    }
                    break;
                }
            default:
                printf("\nThe entered number is not a choice\n");
                break;
            }

            if(c_2_server.request_type <= 2 && player->request_type != 3) /*Server only accessed if who and challenge flags tripped */
            {
                //send to server
                if (sendto(sockServ, &c_2_server, sizeof(c_2_server), 0,
                           (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
                    DieWithError("sendto() sent a different number of bytes than expected");

                memset(&server,0,sizeof(server));
                //recieve from server
                if ((recvfrom(sockServ, &server, sizeof(server), 0,
                              (struct sockaddr *) &fromAddr, &fromSize)) < 0)
                    DieWithError("recvfrom() failed");

                // display who's online
                if(server.response_type==1)
                {
                    for(i = 1; i<sizeof(server.avail_players)/sizeof(bool); i++)
                    {
                        if(server.avail_players[i]==true && i!=player_id)
                            printf("Player ID, %d is online\n",i);
                    }
                }
            }
            //Setup Stream to client
            if(server.response_type == 2 && (player->request_type !=2) )
            {
                player->player_id = player_id;

                //Create CLient TCP Socket
                if ((playerSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
                    DieWithError("socket() failed");
                /* Construct local address structure */
                memset(&playerAddr, 0, sizeof(playerAddr));   /* Zero out structure */
                playerAddr.sin_family = AF_INET;                /* Internet address family */
                playerAddr.sin_addr.s_addr = inet_addr(servIP); /* local address*/
                playerAddr.sin_port = htons(server.tcp_port);    /*player port from server*/

                //Connect to TCP port
                if (connect(playerSock, (struct sockaddr *) &playerAddr, sizeof(playerAddr)) < 0)
                    DieWithError("connect() failed");
                memset(&p,0,sizeof(p));
                p = *player; //copy from mapped memory to temporary structure

                //send Temporary structure
                if (send(playerSock, &p, sizeof(p), 0) < 0)
                    DieWithError("send() sent a different number of bytes than expected");

                //close TCP Socket
                close(playerSock);
            }
        }
    }
}
