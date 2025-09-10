// common/socket.h
#ifndef SOCKET_H
#define SOCKET_H

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>

constexpr std::size_t MAX_DATA = 1024;  // Se define tamano maximo para buffers

// Mensaje que viaja entre Tenedor y Servidor
struct Socket {
    int  client_id {0};          // Se usa para enrutar al cliente correcto
    char function[16]  {};       // "PING", "GET", "ADD:", "DEL", "STATS", etc.
    char input[MAX_DATA]  {};    // Se usa como argumento de la peticion
    char output[MAX_DATA] {};    // Se usa como texto de respuesta
};

// Helpers seguros (definir una sola vez, no re-declarar en .cpp)

// Se crea request con funcion e input
inline Socket make_request(int cid, const std::string& func, const std::string& in) {
    Socket s{};
    s.client_id = cid;
    std::snprintf(s.function, sizeof(s.function), "%s", func.c_str());
    std::snprintf(s.input,    sizeof(s.input),    "%s", in.c_str());
    return s;
}

// Se crea response con output
inline Socket make_response(int cid, const std::string& out) {
    Socket s{};
    s.client_id = cid;
    std::snprintf(s.output, sizeof(s.output), "%s", out.c_str());
    return s;
}

// Se crea sentinela para finalizar consumidores bloqueados en dequeue()
inline Socket make_sentinel() {
    Socket s{};
    s.client_id = -1;
    std::snprintf(s.function, sizeof(s.function), "%s", "__SENTINEL__");
    return s;
}

// Se verifica si un mensaje es sentinela
inline bool is_sentinel(const Socket& s) {
    return s.client_id == -1;
}

#endif // SOCKET_H
