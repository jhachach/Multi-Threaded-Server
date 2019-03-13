/******************************************************************
* 	Author:  Jason Hachach
*	Class: 	 COSC 3336
*	Due: 	 2/23/2018
*	Purpose: A client program that will connect with the server 
*			 and send it requests for the average early career  
*			 and mid-career pays for a specific college major
******************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

void error(string msg)
{
    cout << msg << endl;
    exit(1);
}
struct salary
{
	string major, earlySal, midSal;
};
salary search(vector<salary>, string);

int main()
{
    int sockfd, newsockfd, portno, clilen, pid;
	int earlySal, mid;
	int n;
	int j = 0;
    char buffer[256];
    char reply[16];
    struct sockaddr_in serv_addr, cli_addr;
    salary send;
	string line;
    vector<salary> major;
	fstream inFile;
    string fileName;
  
// Prompt user for input file  
    cout << "Enter the name of the file you wish to open: ";
    getline(cin, fileName);
    inFile.open(fileName.c_str());
    
//	See if file exists
	if(!inFile)
	{
		cout << "Unable to open file " << fileName;
		exit(1);
	}
// Process file line by line by storing individual characters until a tab is reached	
	while(getline(inFile, line))
	{
		int i = 0;
		
		string early, avg, schMajor = "";
		while(line.at(i) != '\t')
		{
			schMajor += line.at(i);
			i++;
		}
		i++;
		while(line.at(i) != '\t')
		{
			early += line.at(i);
			i++;
		}
		i++;
		while(i < line.length())
		{
			avg += line.at(i);
			i++;
		}

// Store strings into vector		
		avg = avg.substr(0, avg.size()-1);
		early = early.substr(0, avg.size()-1);
		major.push_back(salary());
		major[j].major= schMajor;
		major[j].earlySal = early;
		major[j].midSal = avg;
		j++;
	}
	
	cout << "Enter the server port number: ";
	cin >> portno;


// Infinite loop, will only exit when program is manually exited	

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");	    
    bzero((char *) &serv_addr, sizeof(serv_addr));
  
		serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = INADDR_ANY;
    	serv_addr.sin_port = htons(portno);
	
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR on binding");
    listen(sockfd,5);
	
	    clilen = sizeof(cli_addr); 
	while(1)
	{ 
//	Client connected and ready for input	    
newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
	     
	    if (newsockfd < 0) 
	    	error("ERROR on accept");

// Reset buffer
		bzero(buffer,256);
		
// For
		pid = fork();
     	if (pid < 0)
         error("ERROR on fork");
     	if (pid == 0)  
	 	{
         	close(sockfd);
         
// Read from client     
		    n = read(newsockfd,buffer,255);
		    if (n < 0) 
				error("ERROR reading from socket");

// Search for major	    
			send = search(major, (string)buffer);

// Convert string to int
			stringstream e(send.earlySal);
			stringstream a(send.midSal);
			e >> earlySal;
			a >> mid;
		
// Format int to a formatted string to send back to client
		    sprintf(reply, "%7d %7d", earlySal, mid);
		    n = write(newsockfd,reply,15);
	    
			if (n < 0) 
				error("ERROR writing to socket");
			exit(0);
	 	}
     	else 
// Close connection with client			 
			close(newsockfd);
	}	
     return 0; 
}

// Searches a vector of structs to see if the major 
// that was sent by client exists and return a struct 
// with early and average mid salary inside
salary search(vector<salary> searchVector, string sMajor)
{
	salary result;
	result.earlySal = "-1";
	result.midSal = "-1";
	for(int i = 0; i < searchVector.size(); i++)
	{
		if(searchVector[i].major.compare(sMajor) == 0)
		{
			result.earlySal = searchVector[i].earlySal;
			result.midSal = searchVector[i].midSal;
		}
			
	}
	return result;
}
