#include <iostream>
#include <netinet/in.h>
using namespace std;

int main(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(25565);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if(::bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0) cout << "fatal error: failed to bind\n";
    if(listen(serverSocket, 5) != 0) cout << "fatal error: failed to listen\n";

    int clientSocket = accept(serverSocket, nullptr, nullptr);

    uint8_t playerId[131] = {};
    recv(clientSocket, playerId, sizeof(playerId), 0);
    cout << playerId << endl;

    return 0;
}
