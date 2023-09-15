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
#include "gamephysics.hpp"


using json = nlohmann::json;
using namespace gamephysics;
using namespace std::chrono;

std::map<int, sockaddr_in> clients_socket_info;
std::map<int, std::string> world_geoloc_info;
std::string world_data;

std::mutex mtx;
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

void *client_reader_handler(void *arg) {
    while (true) {
        for (auto const& [client_socket, client_address] : clients_socket_info) {
            mtx.lock();
            char read_buffer[1024] = {0};
            read(client_socket, read_buffer, sizeof(read_buffer));
            std::cout << "Received: " << read_buffer << std::endl;
            std::string data = read_buffer;
            world_geoloc_info[client_socket] = data;
            mtx.unlock();
            sleep(1);
        }
    }
}

void *client_writer_handler(void *arg) {
    float heartbeat = 1000.0;
    uint64_t ms0 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    // get time as close to xxxxx0000 as possible
    float ms_to_start = 1000 - ms0 % 1000;
    ms0 = ms0 + ms_to_start;
    while (true) {
        uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        if (ms - ms0 >=  heartbeat) {
            mtx.lock();
            for (auto const& [client_socket, client_address] : clients_socket_info) {
                char write_buffer[1024] = {0};
                std::strcpy(write_buffer, world_data.c_str());
                send(client_socket, write_buffer, strlen(write_buffer), 0);
                std::cout << "Sending message to: " << client_socket << std::endl;
            }
            mtx.unlock();
        }
        ms0 = ms;
    }
}

void *calculate_motion_handler(void *arg) {
    while (true) {
        sleep(.5); // not calculated constantly but faster than the heartbeat
        uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        json data = {};
        for (auto const& [client_socket, motion_vector] : world_geoloc_info) {
            mtx.lock();
            data[client_socket] = motion_vector;
            world_data = to_string(data);
            mtx.unlock();
        }
    }
}


int main() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);
    std::cout << "Server is listening on port 8080..." << std::endl;
    
    pthread_t client_listener_t;
    pthread_create(&client_listener_t, NULL, client_listener_handler, {});

    pthread_t client_reader_t;
    pthread_create(&client_reader_t, NULL, client_reader_handler, {});

//    pthread_t client_writer_t;
//    pthread_create(&client_writer_t, NULL, client_writer_handler, {});

    pthread_t calculate_motion_t;
    pthread_create(&calculate_motion_t, NULL, calculate_motion_handler, {});
    
    while(true) {
        float heartbeat = 1000.0;
        uint64_t ms0 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        // get time as close to xxxxx0000 as possible
        float ms_to_start = 1000 - ms0 % 1000;
        ms0 = ms0 + ms_to_start;
        while (true) {
            uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
            if (ms - ms0 >=  heartbeat) {
                for (auto const& [client_socket, client_address] : clients_socket_info) {
                    char write_buffer[1024] = {0};
                    std::strcpy(write_buffer, world_data.c_str());
                    send(client_socket, write_buffer, strlen(write_buffer), 0);
                    std::cout << "Sending message to: " << client_socket << std::endl;
                }
            }
            ms0 = ms;
        }
}

    return 0;
}
