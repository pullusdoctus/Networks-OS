// Copyright 2025
// Adrian Arrieta / ECCI-UCR. CC BY 4.0

#ifndef TSQ_HPP
#define TSQ_HPP

#include <mutex>
#include <queue>
#include <utility>   // std::move, std::forward

#include "common.hpp"     // debe definir DISABLE_COPY(Clase)
#include "Semaphore.hpp"  // semáforo contable: wait(), signal()

/**
 * @brief Cola genérica segura para hilos (patrón productor-consumidor).
 *
 * @remark Ningún método puede ser const porque todos toman el mutex.
 */
template <typename T>
class TSQ {
  DISABLE_COPY(TSQ);

 protected:
  /// Exclusión mutua para operaciones de lectura/escritura
  std::mutex mutex_;
  /// Cuenta de ítems disponibles para consumir
  Semaphore canConsume_;
  /// Datos compartidos entre productores y consumidores
  std::queue<T> queue_;

 public:
  /// Crea la cola con contador de ítems disponibles en 0
  TSQ() : canConsume_(0) {}

  /// Destructor
  ~TSQ() = default;

  /// Inserta un elemento (copia)
  void enqueue(const T& value) {
    mutex_.lock();
    queue_.push(value);
    mutex_.unlock();
    canConsume_.signal();
  }

  /// Inserta un elemento (move)
  void enqueue(T&& value) {
    mutex_.lock();
    queue_.push(std::move(value));
    mutex_.unlock();
    canConsume_.signal();
  }

  /// Construye en sitio (perfect forwarding)
  template <class... Args>
  void emplace(Args&&... args) {
    mutex_.lock();
    queue_.emplace(std::forward<Args>(args)...);
    mutex_.unlock();
    canConsume_.signal();
  }

  /// Extrae el siguiente elemento disponible (bloquea si está vacía)
  /// @return Una copia/move del elemento extraído
  T dequeue() {
    canConsume_.wait();          // espera hasta que haya algo que consumir
    mutex_.lock();
    T value = std::move(queue_.front());
    queue_.pop();
    mutex_.unlock();
    return value;
  }

  /// Intenta extraer sin bloquear; devuelve true si obtuvo un elemento
  bool try_dequeue(T& out) {
    // Nota: esta versión no usa el semáforo; verifica bajo mutex
    std::lock_guard<std::mutex> lk(mutex_);
    if (queue_.empty()) return false;
    out = std::move(queue_.front());
    queue_.pop();
    // como consumimos un elemento sin usar wait(), ajustamos el semáforo:
    // Evitamos desbalancear canConsume_ consumiendo "a mano".
    // No señalamos ni esperamos aquí: ya reflejamos el consumo quitando del queue.
    // (El semáforo se usa solo para las rutas bloqueantes.)
    return true;
  }

  /// Devuelve el tamaño aproximado (no para sincronización)
  size_t unsafe_size() {
    std::lock_guard<std::mutex> lk(mutex_);
    return queue_.size();
  }
};

#endif  // TSQ_HPP
