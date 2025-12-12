#include <iostream>
#include <netinet/in.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <curl/curl.h>
using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool sendHeartbeat(const string& name, int port, int users, int max, const string& salt, bool isPublic) {
    CURL* curl = curl_easy_init();
    if(!curl) return false;

    string response;
    string url = "http://www.classicube.net/server/heartbeat?name=" + name + 
                 "&port=" + to_string(port) + 
                 "&users=" + to_string(users) + 
                 "&max=" + to_string(max) + 
                 "&salt=" + salt + 
                 "&public=" + (isPublic ? "true" : "false");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if(res == CURLE_OK) {
        cout << "Heartbeat sent. Server URL: " << response << endl;
        return true;
    }
    return false;
}

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

    sendHeartbeat("ccraft", 25565, 0, 0, "gregorywashere", true);

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

        if(bytesReceived < 130){
            cout << "Player id receiving: Buffer error: expected 130 bytes but got " << bytesReceived << endl;
            close(clientSocket);
            continue;
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
            cout << "Player identification packet id ERR: expected packet id with value 0x00 but got 0x" 
                << hex << (int)playerId.packet_id << dec << ". Closing connection.\n";
            close(clientSocket);
            continue;
        }

        if(playerId.prot_ver == 0x07){
            cout << "Player identification protocol version OK\n";
        } else {
            cout << "Player identification protocol version ERR: expected protocol version 0x07 but got 0x" 
                << hex << (int)playerId.prot_ver << dec << ". Closing connection.\n";
            close(clientSocket);
            continue;
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
        serverId.usrtype = 0x00;

        serverIdBuf[0] = serverId.packet_id;
        serverIdBuf[1] = serverId.prot_ver;

        // the rest is filled with 0x20 (ASCII spaces) instead of null-terminators (0x00)
        // for stability across both Notch's original Minecraft 0.30c (which crashes on 0x00s, accepts only 0x20s) and ClassiCube (allows both 0x00s and 0x20s)
        memset(&serverIdBuf[2], 0x20, 64);
        memset(&serverIdBuf[66], 0x20, 64);

        const char* name = "Hello, this is dawidg81. If you see this, it means that it WORKS";
        const char* motd = "almost works... https://github.com/dawidg81/ccraft";
        memcpy(&serverIdBuf[2], name, strlen(name));
        memcpy(&serverIdBuf[66], motd, strlen(motd));

        serverIdBuf[130] = serverId.usrtype;
        cout << "Send server id: Server id buffer written\n";
        
        if(send(clientSocket, serverIdBuf, sizeof(serverIdBuf), 0) < 0){
            cout << "Send server id: Error: failed to send server identification packet. This is fatal to the connection. It will be closed.\n";
            close(clientSocket);
        } else {
            cout << "Send server id: Success.\n";
        }

        uint8_t level_init_packet = 0x02;
        send(clientSocket, &level_init_packet, sizeof(level_init_packet), 0);
        
        /*
        for(ssize_t i = 0; i < sizeof(serverIdBuf); i++){
            printf("%02x ", serverIdBuf[i]);
        }
        */
    }
    return 0;
}
