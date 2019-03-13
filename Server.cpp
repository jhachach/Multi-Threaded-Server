/*********************************************************************
* 	Author: Jason Hachach
*	Class: COSC 3336
*	Due: 4/09/2018
*	Purpose: A server program that will wait for connection requests 
*			 from the client and average early career and mid-career 
*			 pay for the requested college major. 	
*********************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

using namespace std;

void error(string msg)
{
    cout << msg;
    exit(0);
}

int main()
{
    int sockfd, portno, n;
	int earlySal, midSal;
	
    struct sockaddr_in serv_addr;
    struct hostent *server;
	char servName[9];
    char buffer[256];
    char answer[16];
    string majorInput;
	
	do
	{
		majorInput = "";
		bzero(buffer,256);
		
// Prompt user for input of server 
		cout << "Enter the server name you wish to connect to: ";
		cin >> servName;
		cout << "Enter the portnumber: ";
		cin >> portno;
		cout << endl;
	
	    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	    if (sockfd < 0) 
	        error("ERROR opening socket");
	    server = gethostbyname(servName);
// Check if server will open
	    if (server == NULL) 
		{
	        fprintf(stderr,"ERROR, no such host\n");
	        exit(0);
	    }
	    bzero((char *) &serv_addr, sizeof(serv_addr));
	    serv_addr.sin_family = AF_INET;
	    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
	    serv_addr.sin_port = htons(portno);
	    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
	        error("ERROR connecting");

// If client connects to server prompt input of college major
	    cin.ignore();    
	    cout << "Please enter the the college major: ";
		getline(cin, majorInput);

// If input is blank exit client
		if(majorInput.compare("") == 0)
		{
			cout << "Client exiting." << endl;;
			exit(0);
		}

// Convert string to char[]
		for(int i = 0;i < majorInput.length(); i++)
			buffer[i] = majorInput.at(i);

// Write major to server		
	    n = write(sockfd,buffer,strlen(buffer));
	    
	    if (n < 0) 
	         error("ERROR writing to socket");    
	         
// Read from server 
	    n = read(sockfd,answer,255);

// convert answer string to integer
	    sscanf(answer,"%d %d", & earlySal, & midSal);
	    if (n < 0) 
	         error("ERROR reading from socket");
	         
// Check is major exists by seeing if the salaries are -1 if not output salaries 
	    if(earlySal == -1 && midSal == -1)
	    	cout << "Major not found." << endl << endl;

	    else
	    	cout << "The major " << buffer << " has an early salary of $" << earlySal << " and an average salary of $" << midSal << endl << endl;
	}while(majorInput.compare("") != 0);
    return 0;
}

