#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <vector>
using namespace std;
int main(int argc,char *argv[]){

    int fd_client, fd_server;
    int portNum = 1500;
    if(argc>1){
        portNum=atoi(argv[1]);
    }
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];
    int maxClients=100;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len=sizeof(client_addr);

    fd_server = socket(AF_INET, SOCK_STREAM, 0);

    if (fd_server < 0)
    {
        cout << "\nError establishing socket..." << endl;
        exit(1);
    }
    cout << "success: established socket..." << endl;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    if ((bind(fd_server, (struct sockaddr*)&server_addr,sizeof(server_addr))) < 0)
    {
        cout << "=> Error binding connection, the socket has already been established..." << endl;
        close(fd_server);
        exit(1);
    }
    cout << "success: bind connection..." << endl;
    if(listen(fd_server, 100)==-1){
        cout << "=> Error listening..." << endl;
        close(fd_server);
        exit(1);
    }
    cout << "listening..." << endl;
    while(true){
        fd_client= accept(fd_server,(struct sockaddr *)&server_addr,&len);
        if(fd_client== -1){
            cout << "error accepting"<<endl;
            continue;
        }
        strcpy(buffer, "=> Server connected...\n");
        send(fd_client, buffer, bufsize, 0);
        cout << "success: client connected..." << endl;
        int pid=fork();
        if(!pid){
            cout << "in child process..."<<getpid() << endl;
            int read_size;
            if(read_size=recv(fd_client, buffer, bufsize-1, 0)>0){
                cout <<"=>"<< buffer << endl;
                vector<string> vec;
                char * pch;
                pch = strtok (buffer," ");
                while (pch != NULL){
                    vec.push_back(pch);
                    pch = strtok (NULL, " ");
                }
                if(vec[0].compare("GET")==0){
                    //while not empty line received
                        //print the optional commands
                    //try to open file
                    //if not found
                        // send 404
                    //else
                        // send 200
                        //read file into buffer and send
                        //send blank line

                }else if(vec[0].compare("POST")==0){
                    //send 200
                    //receive all chunks of the file
                    //save file

                }

            }
            else if(read_size == 0){
                cout <<"Client disconnected"<<endl;
            }
            else if(read_size == -1){
               cout <<"recv failed"<<endl;
            }
            close(fd_client);
            close(fd_server);
            exit(0);
        }
        close(fd_client);
    }
close(fd_client);
}
