// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <semaphore.h>

#include "common.hpp"

// macOS only uses named semaphores
#if __APPLE__ && __MACH__
  #define USE_NAMED_SEMAPHORE 1
#endif

#ifdef USE_NAMED_SEMAPHORE
  #define SEM_NAME_PATTERN "/tmp/semXXXXXX"
  #define SEM_NAME_LEN 15  // strlen(SEM_NAME_PATTERN) + 1
#endif

/// Wrapper class for POSIX semaphores
class Semaphore {
  DISABLE_COPY(Semaphore);

 private:
#if USE_NAMED_SEMAPHORE
  /// The file name of the semaphore
  char name[SEM_NAME_LEN];
  /// Pointer to a thread-safe integer value
  sem_t* semaphore;
#else
  /// A thread-safe integer value
  sem_t semaphore;
#endif

 public:
  /// Constructs a semaphore with the given value
  explicit Semaphore(unsigned value);
  /// Destructor
  ~Semaphore();
  /// Increment the semaphore and wake a waiting thread if any
  void signal();
  /// Decrement the semaphore and block if the result is negative
  void wait();
  /// Try to decremet the semaphore without blocking
  bool try_wait();
};

#endif  // SEMAPHORE_HPP