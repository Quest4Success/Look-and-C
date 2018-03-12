
Client and Server files included in this file were tested and run on OS X machines.

To compile the server program type gcc -o *Compilation Name* UDPEchoServer.c DieWithError.c 
in terminal.

To begin the server type ./UDPEchoServer 3456 in terminal after compilation. Second Number
is port if no port then 7 is entered.

To compile type gcc -o *Compilation Name* UDPEchoClient.c DieWithError.c in terminal

To run program type ./*Compilation Name* 127.0.0.1 3456 in the terminal After compilation. 
The first case has to be IP and second the port. 7 is automatically the port if not specified.
This should be done after Server is running, to actually send messages.

The Client program runs a menu that will prompt the user to login,follower a leader, 
post a message, recieve a feed from all leaders followed, Search for any post matching 
a case string, delete the first message that matches a given case string, unfollow leaders
logout, and exit the mune and close the program completely

*Note when searching for post match a tag or deleting a post, the user must input 
the first string within the post. The program matches this completely so spelling is 
important as well as the case of each character in the string. The post marked for 
deletion will be the first message that is found with the case, so if you want to use
a tag to specify a specific post, that tag must be at the beginning of the message at the 
time of posting.

enjoy!
