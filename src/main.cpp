#include <iostream>
#include <netinet/in.h>
#include <string>
#include "main.h"
using namespace std;

int main(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(25565);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(::bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0){
        cout << "Fatal error: failed to bind to the address. Exiting with code 1.\n";
        return 1;
    }

    if(listen(serverSocket, 5) != 0){
        cout << "Fatal error: failed to listen to port. Exiting with code 2.\n";
        return 2;
    }

    while(true){
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        uint8_t playerIdBuf[131];
        ssize_t bytesReceived = recv(clientSocket, playerIdBuf, sizeof(playerIdBuf) - 1, 0);

        if(bytesReceived > 0){
            cout << "Incoming connection. Received " << bytesReceived << " bytes. Printing player identification buffer." << endl;
            cout << "HEX: ";
            for(ssize_t i = 0; i < bytesReceived; i++){
                printf("%02x ", playerIdBuf[i]);
            }
            cout << endl;
            playerIdBuf[bytesReceived] = '\0';
            cout << "ASCII: '" << (char*)playerIdBuf << "'" << endl;
        }

        struct playerIdPack{
            uint8_t packet_id;
            uint8_t prot_ver;
            string username;
            string ver_key;
            uint8_t unused;
        };

        playerIdPack playerId;
        playerId.packet_id = playerIdBuf[0];
        playerId.prot_ver = playerIdBuf[1];
        // TODO: string username: how to put a set of bytes from playerIdBuf table into string?
        // TODO: string ver_key: how to put a set of bytes from playerIdBuf table into string?
        playerId.unused = playerIdBuf[130];

        if(playerId.packet_id == 0x00){
            cout << "Player identification packet id OK\n";
        } else {
            cout << "Player identification packet id ERROR: expected packet id with value 0x00 but got " << playerId.packet_id << ". Closing connection.\n";
            close(serverSocket);
        }

        if(playerId.prot_ver == 0x07){
            cout << "Player identification protocol version OK\n";
        } else {
            cout << "Player identification protocol version ERROR: expected protocol version 0x07 but got " << playerId.prot_ver << ". Closing connection.\n";
            close(serverSocket);
        }
    }
    return 0;
}
