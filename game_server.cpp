//
//  game_server.cpp
//  joming-server
//
//  Created by Jocelyn Chang on 8/31/23.
//
// socket code taken from https://marketsplash.com/tutorials/cpp/cplusplus-scoket/

#include "game_server.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using json = nlohmann::json;

namespace gamephysics {
    struct PositionVector {
        float x;
        float y;
        float z;
    };
    struct VelocityVector {
        float x;
        float y;
        float z;
    };

    void to_json(json& j, const PositionVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, PositionVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }

    void to_json(json& j, const VelocityVector& p) {
        j = json{ {"x", p.x}, {"y", p.y}, {"z", p.z} };
    }

    void from_json(const json& j, VelocityVector& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
        j.at("z").get_to(p.x);
    }
}


int main() {
    using namespace std::chrono;

    int server_socket, client_socket;
    sockaddr_in server_address, client_address;
    socklen_t client_length;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);
    
    std::cout << "Server is listening on port 8080..." << std::endl;
    
    uint64_t ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    client_length = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_length);

    for (;;) {
        char buffer[1024] = {0};
        read(client_socket, buffer, sizeof(buffer));
        std::cout << buffer << std::endl;
        if (strcmp(buffer, "\\q")==0) {
            std::cout << "Closing socket" << std::endl;
            close(client_socket);
            close(server_socket);
            return 0;
        } else {
            json j = json::parse(buffer);
            std::cout << "Received: " << j.at("y") << std::endl;
            send(client_socket, "hello client", strlen(buffer), 0);
        }
    }
    return 0;
}
