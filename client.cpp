
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
        cin >> buffer;
        vector<string> vec; 
          char * pch;
          pch = strtok (buffer," ");
          while (pch != NULL)
          {
            vec.push_back(pch);
            pch = strtok (NULL, " ");
          }
          if(vec[0].compare("get"))
          {
        cout << "Client: ";
        printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
   int n = write(client, buffer, strlen(buffer));
    if (n < 0) 
        printf("ERROR writing to socket");
    bzero(buffer,256);
    n = read(client, buffer, 255);
    if (n < 0) 
         printf("ERROR reading from socket");
    printf("%s\n", buffer);}
    else if(vec[0].compare("post"))
          {
        cout << "Client: ";
        printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
   int n = write(client, buffer, strlen(buffer));
    if (n < 0) 
        printf("ERROR writing to socket");
    bzero(buffer,256);
    n = read(client, buffer, 255);
    if (n < 0) 
         printf("ERROR reading from socket");
    printf("%s\n", buffer);}


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