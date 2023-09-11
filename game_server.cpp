//
//  game_server.cpp
//  joming-server
//
//  Created by Jocelyn Chang on 8/31/23.
//
// socket code taken from https://marketsplash.com/tutorials/cpp/cplusplus-scoket/
/*
 https://stackoverflow.com/questions/42643276/how-to-connect-multiple-clients-to-a-single-server-in-c-on-windows-visual-st
 You want one thread to open the listening socket and wait for incoming connections there. Note that you cannot have more than one listening socket per port, so you definitely don't want multiple threads trying to listen on the same port simultaneously.

 If a connection comes in, accept will give you a new socket object. You still have the original listening socket, which waits for new connections, but you also have a second socket now with an already established connection to a client.

 Now you can split the work: Have one thread go back to calling listen on the original socket and await new connections, while the other thread grabs the new socket and performs the necessary I/O to interact with the client.

 In this simple scheme, you will always have one thread per client connection plus an additional thread for the listening socket. Since all of these threads will spent a large amount of time just waiting for network I/O to complete, you can use asynchronous I/O to share the workload between fewer (or even a single) threads, but that is slightly more complex to pull off, so I'd suggest you leave that for a second draft.
 */


#include "game_server.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <mutex>
#include <pthread.h>
#include <fcntl.h>


using json = nlohmann::json;


std::map<int, sockaddr_in> clients_socket_info;
std::map<int, sockaddr_in> world_geoloc_info;

std::mutex client_info_mutex;
int server_socket;
sockaddr_in server_address;
int count = 0;
void *client_listener_handler(void *arg) {
    while (true) {
        int client_socket;
        sockaddr_in client_address;
        socklen_t client_length;
        
        client_length = sizeof(client_address);
        
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_length);
        fcntl(client_socket, F_SETFL, O_NONBLOCK);
//        std::lock_guard<std::mutex> guard(client_info_mutex);
            clients_socket_info[client_socket] = client_address;
        std::cout << "client connected at " + std::to_string(client_socket) << std::endl;
    }
}


int main() {
    using namespace std::chrono;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);
    std::cout << "Server is listening on port 8080..." << std::endl;
    
    pthread_t client_listener_t;
    pthread_create(&client_listener_t, NULL, client_listener_handler, {});

    while(true) {
        uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        //        if (ms % 150 == 0 ) {
            for (auto const& [client_socket, client_address] : clients_socket_info) {
                char read_buffer[1024] = {0};
                read(client_socket, read_buffer, sizeof(read_buffer));
//                json j = json::parse(buffer);
//                std::cout << "Received: " << j.at("y") << std::endl;
                std::cout << "Received: " << read_buffer << std::endl;
//                if (ms % 150 == 0 ) {
                sleep(1);
                    char write_buffer[1024] = "ACK";
                    send(client_socket, write_buffer, strlen(write_buffer), 0);
                    std::cout << "Sending message to: " << client_socket << std::endl;
//                }
            }
//        }
    }

    return 0;
}
