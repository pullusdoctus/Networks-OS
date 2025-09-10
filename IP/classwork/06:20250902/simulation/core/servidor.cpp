#include "servidor.hpp"
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iostream>

//Constructor del Servidor, 2 colas para consumir y producir
Servidor::Servidor(std::shared_ptr<TSQ<Socket>> reqQ,
                   std::shared_ptr<TSQ<Socket>> respQ) {
    // Inicializar las colas
    this->reqQ_ = reqQ;
    this->respQ_ = respQ;

    // Figuras precargadas en la "Datablock"
    store_["triangle"] =
        "  /\\\n"
        " /  \\\n"
        "/____\\\n";
}


// Se encola sentinela en reqQ para terminar
void Servidor::stop() {
    reqQ_->enqueue(make_sentinel());
}

// Bucle principal del hilo: Se procesa request y se envia respuesta
int Servidor::run() {
    running_ = true;
    for (;;) {
        // Se consume request (bloquea si vacia)
        Socket req = reqQ_->dequeue();
        if (is_sentinel(req)) break;  // Se termina por sentinela

        // Se extraen campos
        std::string func(req.function);
        std::string input(req.input);
        const int cid = req.client_id;
        std::cout << "[Servidor] Se consumio socket del cliente " << cid << " solicitando " << func <<std::endl;
        // Se atiende request
        const std::string out = handle(func, input, cid);

        // Se arma respuesta para el cliente
        Socket resp;
        resp.client_id = cid;
        std::strncpy(resp.function, "RESP", sizeof(resp.function)-1);
        std::strncpy(resp.output,   out.c_str(), sizeof(resp.output)-1);

        // Se envia respuesta al Tenedor
        respQ_->enqueue(resp);
    }
    running_ = false;
    return 0;
}

// Se atienden comandos basicos: PING, LIST, GET, ADD:, DEL, STATS
std::string Servidor::handle(const std::string& fn, const std::string& in, int cid) {
    requests_++;

    if (fn == "PING") {
        return "[Servidor] PONG\n";
    }

    if (fn == "LIST") {
        std::cout << "[Servidor] Se reconocio comando LIST, procediendo a regresar el socket a Tenedor." << std::endl;

        std::string result;
        result += "[Servidor] LIST\n";

        // Se concatena el contenido del database a result
        {
            std::lock_guard<std::mutex> lk(m_);
            if (store_.empty()) {
                result += "[Servidor] (empty)\n";
            } else {
                for (auto it = store_.begin(); it != store_.end(); ++it) {
                result += " - " + it->first + "\n";
                }
            }
        }

        return result;
    }

    if (fn == "GET") {
        // in =  nombre de la figura
        std::string body;
        {
            std::lock_guard<std::mutex> lk(m_);
            auto it = store_.find(in);
            if (it != store_.end()) body = it->second;
        }
        if (body.empty()) {
            return "[Servidor] ERR la figura " + in + " no se encontro\n";
        }


        std::string result;
        result += "[Servidor] OK " + in + "\n";
        result += body;
        if (body.empty() || body.back() != '\n') {
            result += "\n";
        }
        return result;

    }

    if (fn == "ADD") {
        // in = "name|body"
        auto pos = in.find('|');
        if (pos == std::string::npos) return "ERR bad format, expected name|body\n";
        std::string clave = in.substr(0, pos);
        std::string valor = in.substr(pos + 1);

        {
            std::lock_guard<std::mutex> lk(m_);
            store_[clave] = valor;
        }
        return "[Servidor] OK added " + clave + "\n";
    }

    if (fn == "DEL") {
        // in = nombre
        bool erased = false;
        {
            std::lock_guard<std::mutex> lk(m_);
            if (store_.erase(in) > 0) {
                erased = true;
            } else {
                erased = false;
            }
        }
        if (erased) {
            return "[Servidor] OK " + in + " deleted\n";
        } else {
            return "[Servidor] ERR not found\n";
        }
    }

    if (fn == "STATS") {
        size_t size = 0;
        {
            std::lock_guard<std::mutex> lk(m_);
            size = store_.size();
        }

        std::string result;
        result += "[Servidor] STATS req=";
        result += std::to_string(requests_.load());
        result += " items=";
        result += std::to_string(size);
        result += "\n";

        return result;
    }

    // Comando no soportado
    return "[Servidor] ERR unknown command\n";
}
  