#include <iostream>
#include <thread>
#include <chrono>
#include "../common/tsq.hpp"     // Cola TSQ basada en semáforos
#include "../common/socket.h"
#include "../core/tenedor.hpp"
#include "../core/servidor.hpp"
#include "../common/Client.hpp"    // <-- tu clase Client

int main() {
    // Se crean 2 colas compartidas
    auto reqQ  = std::make_shared<TSQ<Socket>>();
    auto respQ = std::make_shared<TSQ<Socket>>();

    // Se crean Tenedor y Servidor con referencias compartidas a las colas
    Tenedor  tenedor(reqQ, respQ);
    Servidor servidor(reqQ, respQ);

    // Se inician los hilos
    servidor.startThread();  // hilo del servidor
    tenedor.startThread();   // hilo del tenedor

    // Se crean clientes simulados
    Client c1(1);
    Client c2(2);
    Client c3(3);

    c1.send(tenedor, "PING");
    c2.send(tenedor, "GET triangle");
    // c3 envia un ASCII-art
    std::string duck =
        "  __\n"
        "<(o )___\n"
        " ( ._> /\n"
        "  `---'";
    c3.send(tenedor, std::string("ADD duck|") + duck);
    c3.send(tenedor, "GET duck");
    c1.send(tenedor, "LIST");
    c2.send(tenedor, "STATS");

    // Espera la terminación de los hilos
    servidor.waitToFinish();
    tenedor.waitToFinish();

    // Cierre ordenado con sentinelas
    servidor.stop();     // Encola sentinela en reqQ
    tenedor.stop();      // Encola sentinela en respQ

    return 0;
}
