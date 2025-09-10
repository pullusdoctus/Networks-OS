#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
class Tenedor;

class Client {
private:
    int id;

public:
    explicit Client(int clientId);
    ~Client();

    int getId() const;

    // El cliente envía un texto al Tenedor (ADD/GET/LIST/DEL/STATS)
    void send(Tenedor& tenedor, const std::string& text);

    // El Tenedor le entrega la respuesta al cliente
    void on_response(const std::string& text);
};

#endif // CLIENT_HPP
