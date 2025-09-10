#include "Client.hpp"
#include "../core/tenedor.hpp"   // implementación necesita tipo completo
#include <iostream>

Client::Client(int clientId) {
    this->id = clientId;
}

Client::~Client() {
    // destructor vacio
}

int Client::getId() const {
    return this->id;
}

void Client::send(Tenedor& tenedor, const std::string& text) {
    // Pasa su propio puntero para que Tenedor pueda responderle luego
    tenedor.submit_text(id, text, this);
}

void Client::on_response(const std::string& text) {
    std::cout << "[C" << id << " <-] " << text;
    if (!text.empty() && text.back() != '\n') std::cout << "\n";
}