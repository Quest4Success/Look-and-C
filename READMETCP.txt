
Client and Server files included in this file were tested and run on OS X machines.

To compile the server program type gcc -o *Compilation Name* UDP_TCPServer.c DieWithError.c 
in terminal.

To begin the server type ./UDPEchoServer *port Number* in terminal after compilation.

To compile type gcc -o *Compilation Name* TCPClient.c DieWithError.c in terminal

To run program type ./*Compilation Name* *address* *server port number* *client to client port* in the terminal After compilation. 
Ports and Addresses have to be specified and match the oppoisng connect server or client. This should be done after Server is running, to actually send messages.

The Client program will prompt the user to login. Then proceed to menu with a who,challenge,and logout options.
This will be done in tandem with a forkd listen process as well as memory mapped Objects;After the listening process has recieved a message it will prompt for user input.
This input has to be first accessed by clicking 4 for a second menu. This menu drives
client to client iteractions and should only be used after a challenge has been issued or accepted.

*Note 
Tic Tac Toe is played as a grid first entering a column than a number of horizontal steps to move.

enjoy!
