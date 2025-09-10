#ifndef TENEDOR_HPP
#define TENEDOR_HPP

#include <memory>
#include <unordered_map>
#include <mutex>
#include <string>

#include "Thread.hpp"
#include "tsq.hpp"
#include "socket.h"
class Client;

// Clase Tenedor: recibe requests de clientes y reenvia respuestas
class Tenedor : public Thread {
public:
    // Se guardan referencias compartidas a las colas
    Tenedor(std::shared_ptr<TSQ<Socket>> reqQ,
            std::shared_ptr<TSQ<Socket>> respQ);

    // Se recibe texto de un cliente y se convierte en request hacia el servidor
    void submit_text(int client_id, const std::string& text,
                 Client* client = nullptr);

    // Se encola sentinela en respQ para terminar el bucle de run()
    void stop();

protected:
    // Bucle principal del hilo (router de respuestas Servidor -> Cliente)
    int run() override;

private:
    std::shared_ptr<TSQ<Socket>> reqQ_;
    std::shared_ptr<TSQ<Socket>> respQ_;

    std::unordered_map<int, Client*> clients_;
    std::mutex mutex_;
    bool running_ = false;
};

#endif // TENEDOR_HPP
