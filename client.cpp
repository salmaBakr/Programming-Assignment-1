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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <sstream> 
using namespace std;

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

    if (connect(client,(struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
        cout << "=> Connection to the server port number: " << portNum << endl;


    cout << "=> Awaiting confirmation from the server..." << endl; //line 40
    recv(client, buffer, bufsize, 0);
    cout << "=> Connection confirmed, you are good to go...";

    cout << "\n\n=> Enter # to end the connection\n" << endl;

    // Once it reaches here, the client can send a message first.

    do {
         bzero(buffer,bufsize);
        cout << "Client: ";
            printf("Please enter the message: ");
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
            cout<<str<<endl;
           int n = write(client, str, strlen(str));
            if (n < 0) 
                printf("ERROR writing to socket");
            n = read(client, buffer, bufsize-1);
            if (n < 0) 
                 printf("ERROR reading from socket");
             else
                printf("%s\n", buffer);
           }
    else if(vec[0].compare("post")==0)
          {
            string text;
            stringstream stream;
            FILE *sendFile = fopen(vec[1].c_str(), "r");
            if (sendFile == NULL) /* check it the file was opened */
                return 0;

            fseek(sendFile, 0L, SEEK_END);
            stream << "HTTP/1.1 200 OK\nContent-length: " << ftell(sendFile) << "\n";
            fseek(sendFile, 0L, SEEK_SET);
            text = stream.str();
            send(client, text.c_str(), text.length(), 0);
            std::cout << "Sent : " <<  text << std::endl;
            text = "Content-Type: text/html\n\n";
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


    } while (1);

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