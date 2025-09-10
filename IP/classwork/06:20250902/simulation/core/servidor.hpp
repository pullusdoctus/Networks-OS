#pragma once
#include <memory>
#include <atomic>
#include <string>
#include <unordered_map>
#include <mutex>

#include "../common/tsq.hpp"
#include "../common/socket.h"
#include "../common/Thread.hpp"

// Servidor: consume requests de reqQ y produce respuestas a respQ
class Servidor : public Thread {
public:
    // Se guardan referencias compartidas a las colas
    Servidor(std::shared_ptr<TSQ<Socket>> reqQ,
             std::shared_ptr<TSQ<Socket>> respQ);

    // Se encola sentinela en reqQ para terminar run()
    void stop();

protected:
    // Bucle principal del hilo
    int run() override;

private:
    // Se atiende un comando y se arma respuesta en texto
    std::string handle(const std::string& fn, const std::string& in, int cid);

private:
    std::shared_ptr<TSQ<Socket>> reqQ_;
    std::shared_ptr<TSQ<Socket>> respQ_;
    std::atomic<bool> running_{false};

    // Database de figuras u objetos
    std::unordered_map<std::string, std::string> store_;
    std::mutex m_;

    std::atomic<uint64_t> requests_{0};
};