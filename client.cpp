#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <vector>
#include <sstream> 
#include <fcntl.h> 
#include <sys/time.h>
//Size of each chunk of data received, try changing this
#define CHUNK_SIZE 1024
using namespace std;
int recv_timeout(int s , int timeout,string fileName)
{
    int size_recv , total_size= 0;
    struct timeval begin , now;
    char chunk[CHUNK_SIZE];
    double timediff;
     
    //make socket non blocking
    fcntl(s, F_SETFL, O_NONBLOCK);
     
    //beginning time
    gettimeofday(&begin , NULL);
     
    while(1)
    {
        gettimeofday(&now , NULL);
         
        //time elapsed in seconds
        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
         
        //if you got some data, then break after timeout
        if( total_size > 0 && timediff > timeout )
        {
            break;
        }
         
        //if you got no data at all, wait a little longer, twice the timeout
        else if( timediff > timeout*2)
        {
            break;
        }
         
        memset(chunk ,0 , CHUNK_SIZE);  //clear the variable
        if((size_recv =  recv(s , chunk , CHUNK_SIZE , 0) ) < 0)
        {
            //if nothing was received then we want to wait a little before trying again, 0.1 seconds
            usleep(100000);
        }
        else
        {
            total_size += size_recv;
            printf("%s" , chunk);
            //reset beginning time
            gettimeofday(&begin , NULL);
        }
    }
     
    return total_size;
}
int main(int argc, char *argv[])
{
    

   cout<<"server IP : "<<argv[1]<<endl;
    cout<<"port number : "<<argv[2]<<endl;
    int client;
    int portNum = atoi(argv[2]); // NOTE that the port number is same for both client and server
   // int portNum = 1500; // NOTE that the port number is same for both client and server
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];
    char* ip = argv[1];//"127.0.0.1";
   // char* ip = "127.0.0.1";

    struct sockaddr_in server_addr;

    client = socket(AF_INET, SOCK_STREAM, 0);

    /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
    /* --------------- socket() function ------------------*/

    if (client < 0) 
    {
        cout << "\nError establishing socket..." << endl;
        exit(1);
    }

    cout << "\n=> Socket client has been created..." << endl;
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);


    /* ---------- CONNECTING THE SOCKET ---------- */
    /* ---------------- connect() ---------------- */

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {
            cout << "=> Connection failed to the server port number: " << portNum << endl;
            return 0;
        }
   
    cout << "=> Connection confirmed, you are good to go...";

    cout << "\n\n=> Enter # to end the connection\n" << endl;

    // Once it reaches here, the client can send a message first.
         bzero(buffer,bufsize);
        cout << "Client: ";
        printf("Please enter the command: ");
        fgets(buffer,bufsize-1,stdin);
        vector<string> vec; 
          char * pch;
          pch = strtok (buffer," ");
          while (pch != NULL)
          {
            vec.push_back(pch);
            pch = strtok (NULL, " ");
          }
          if(vec[0].compare("get")==0)
          {
            bzero(buffer,bufsize);
            char str[256];
            strcpy (str,"GET /");
            strcat (str,vec[1].c_str());
            strcat (str," HTTP/1.1\r\nHost: ");
            strcat (str,vec[2].c_str());
           int n = send(client, str, strlen(str),0);
            if (n < 0) 
                printf("ERROR writing to socket");
            n = recv(client, buffer, bufsize-1,0);
            if (n < 0) 
                 printf("ERROR reading from socket");
             else
                {
                    //404
                    cout<<buffer<<endl;
                    if(strcmp ("404",buffer) ==0)
                        printf("File not found");
                    //ok
                    if(strcmp ("OK",buffer) ==0)
                        {
                            int total_recv = recv_timeout(client, 4,vec[1]);
                            printf("\n\nDone. Received a total of %d bytes\n\n" , total_recv);
                    }
                }
           }
    else if(vec[0].compare("post")==0)
          {
            char str[256];
            strcpy (str,"POST /");
            strcat (str," HTTP/1.1\r\nHost: ");
            strcat (str,vec[2].c_str());
           int n = send(client, str, strlen(str),0);
            if (n < 0) 
                printf("ERROR writing to socket");
            n = recv(client, buffer, bufsize-1,0);
            if (n < 0) 
                 printf("ERROR reading from socket");
            string text;
            stringstream stream;
            FILE *sendFile = fopen(vec[1].c_str(), "r");
            if (sendFile == NULL) /* check it the file was opened */
                return 0;

            fseek(sendFile, 0L, SEEK_END);
            stream << "POST HTTP/1.1 200 OK\nContent-length: " << ftell(sendFile) << "\n";
            fseek(sendFile, 0L, SEEK_SET);
            text = stream.str();
            send(client, text.c_str(), text.length(), 0);
            std::cout << "Sent : " <<  text << std::endl;
            text = "Content-Type: text/html/image\n\n";
            send(client, text.c_str(), text.length(), 0);
            std::cout << "Sent : %s" << text << std::endl;
            while (feof(sendFile) == 0)
            {
                int  numread;
                char sendBuffer[500];

                numread = fread(sendBuffer, sizeof(unsigned char), 1024, sendFile);
                if (numread > 0)
                {
                    char *sendBuffer_ptr;
                    sendBuffer_ptr = sendBuffer;
                    do {
                        fd_set  wfd;
                        timeval tm;

                        FD_ZERO(&wfd);
                        FD_SET(client, &wfd);
                        tm.tv_sec  = 10;
                        tm.tv_usec = 0;
                        /* first call select, and if the descriptor is writeable, call send */
                        if (select(1 + client, NULL, &wfd, NULL, &tm) > 0)
                        {
                            int numsent;

                            numsent = send(client, sendBuffer_ptr, numread, 0);
                            if (numsent == -1)
                                return 0;
                            sendBuffer_ptr += numsent;
                            numread        -= numsent;
                        }
                    } while (numread > 0);
                }
            }
            /* don't forget to close the file. */
            fclose(sendFile);
        }

    /* ---------------- CLOSE CALL ------------- */
    /* ----------------- close() --------------- */

    /* 
        Once the server presses # to end the connection,
        the loop will break and it will close the server 
        socket connection and the client connection.
    */

    cout << "\n=> Connection terminated.\nGoodbye...\n";

    close(client);
    return 0;
}