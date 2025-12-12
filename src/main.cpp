#include <iostream>
#include <netinet/in.h>
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

        uint8_t playerId[131];
        ssize_t bytesReceived = recv(clientSocket, playerId, sizeof(playerId) - 1, 0);

        if(bytesReceived > 0){
            cout << "Incoming connection. Received " << bytesReceived << " bytes." << endl;
            cout << "HEX: ";
            for(ssize_t i = 0; i < bytesReceived; i++){
                printf("%02x ", playerId[i]);
            }
            cout << endl;
            playerId[bytesReceived] = '\0';
            cout << "ASCII: '" << (char*)playerId << "'" << endl;
        }
    }

    return 0;
}
