#include "tenedor.hpp"
#include <iostream>
#include <cstring>
#include "Client.hpp" 
// Constructor del tenedor, recibe 2 colas para producir y consumir.
Tenedor::Tenedor(std::shared_ptr<TSQ<Socket>> reqQ,
                 std::shared_ptr<TSQ<Socket>> respQ) {
    this->reqQ_ = reqQ;
    this->respQ_ = respQ;
}


// Metodo que recibe texto de un cliente y lo convierte en request
void Tenedor::submit_text(int client_id, const std::string& text, Client* client) {
    // Se registra cliente si es la primera vez que envia algo
    {
        std::lock_guard<std::mutex> lk(mutex_);
        if (clients_.find(client_id) == clients_.end() && client) {
            clients_[client_id] = client;
        }
    }

    // Se imprime depuracion de lo que envia el cliente
    std::cout << "[C" << client_id << " ->] " << text << std::endl;
    std::cout << "[Tenedor] " << "El cliente pidio: " << text << ", enviando socket al servidor.\n";
    if (text.empty()) {
        std::cout << "Cliente debe introducir una funcion valida\n";
    }

    // Se parsea comando y argumento
    auto parse_cmd = [](std::string s)->std::pair<std::string,std::string> {
        // Si la cadena termina con '\n', se lo quita
        if (!s.empty() && s.back()=='\n') s.pop_back();

        // Se busca el primer espacio en blanco
        auto pos = s.find(' ');

        // Si no hay espacio, todo es comando (sin argumento)
        if (pos == std::string::npos) return {s, ""};

        // Si hay espacio, parte en dos, izq comando, der argumento
        return { s.substr(0,pos), s.substr(pos+1) };
    };

    // Guardamos el comando y argumento
    auto result = parse_cmd(text);
    std::string func = result.first;
    std::string input = result.second;

    // Se crea socket request
    Socket req;
    req.client_id = client_id;
    std::strncpy(req.function, func.c_str(), sizeof(req.function) - 1);
    std::strncpy(req.input, input.c_str(), sizeof(req.input) - 1);

    // Se encola request hacia Servidor (TSQ con semaforo)
    reqQ_->enqueue(std::move(req));
}

// Bucle principal del hilo Tenedor: Se enrutan respuestas a clientes
int Tenedor::run() {
    running_ = true;
    for (;;) {
        // Se consume respuesta de la cola respQ (bloquea si vacia)
        Socket resp = respQ_->dequeue();

        // Se verifica sentinela para terminar (El sentinela es un usuario con id -1)
        if (is_sentinel(resp)) break;

        // Se busca cliente y se entrega respuesta
        std::string text(resp.output);
        {
            std::lock_guard<std::mutex> lk(mutex_);
            auto it = clients_.find(resp.client_id);
            if (it != clients_.end()) {
                it->second->on_response(text);
            } else {
                std::cout << "[TENEDOR] cliente " << resp.client_id << " no registrado\n";
            }
        }
    }
    running_ = false;
    return 0;
}

// Se encola sentinela para terminar el bucle de run()
void Tenedor::stop() {
    respQ_->enqueue(make_sentinel());
}
