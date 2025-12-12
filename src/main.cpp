#include <iostream>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <unistd.h>
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
        // PLAYER IDENTIFICATION
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

        playerId.username = string((char*)&playerIdBuf[2], 64);
        playerId.ver_key = string((char*)&playerIdBuf[66], 64);
        playerId.unused = playerIdBuf[130];

        if(playerId.packet_id == 0x00){
            cout << "Player identification packet id OK\n";
        } else {
            cout << "Player identification packet id ERR: expected packet id with value 0x00 but got " << playerId.packet_id << ". Closing connection.\n";
            close(clientSocket);
        }

        if(playerId.prot_ver == 0x07){
            cout << "Player identification protocol version OK\n";
        } else {
            cout << "Player identification protocol version ERR: expected protocol version 0x07 but got " << playerId.prot_ver << ". Closing connection.\n";
            close(clientSocket);
        }

        // SERVER IDENTIFICATION
        uint8_t serverIdBuf[131];
        cout << "Send server id: Created server id buffer\n";
        struct serverIdPack{
            uint8_t packet_id;
            uint8_t prot_ver;
            char name[64];
            char motd[64];
            uint8_t usrtype;
        };
        cout << "Send server id: Created server id structure\n";
        serverIdPack serverId;
        cout << "Send server id: Created server id\n";
        serverId.packet_id = 0x00;
        serverId.prot_ver = 0x07;
        strncpy(serverId.name, "Hello", sizeof(serverId.name) - 1);
        strncpy(serverId.motd, "If you see this it means that it works", sizeof(serverId.motd) - 1);
        serverId.usrtype = 0x00;
        cout << "Send server id: Written server id\n";
        serverIdBuf[0] = serverId.packet_id;
        serverIdBuf[1] = serverId.prot_ver;

        // FIX: Something fails between line 92 and line 102. To debug.

        memcpy(&serverIdBuf[2], serverId.name, sizeof(serverId.name));
        memcpy(&serverIdBuf[66], serverId.motd, sizeof(serverId.motd));

        serverIdBuf[130] = serverId.usrtype;
        cout << "Send server id: Server id buffer written";
        
        if(send(clientSocket, serverIdBuf, sizeof(serverIdBuf), 0) < 0){
            cout << "Send server id: Error: failed to send server identification packet. This is fatal to the connection. It will be closed.\n";
            close(clientSocket);
        }
        
        /*
        for(ssize_t i = 0; i < sizeof(serverIdBuf); i++){
            printf("%02x ", serverIdBuf[i]);
        }
        */
    }
    return 0;
}
