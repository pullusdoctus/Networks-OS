// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include <unistd.h>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include "Semaphore.hpp"

Semaphore::Semaphore(unsigned value) {
#if USE_NAMED_SEMAPHORE
  // Get an unique filename
  // Fill the patter with a unique name on the system
  std::strncpy(this->name, SEM_NAME_PATTERN, SEM_NAME_LEN);
  int fileDescriptor = ::mkstemp(this->name);
  assert(fileDescriptor >= 0);
  close(fileDescriptor);
  this->semaphore = ::sem_open(this->name, O_CREAT | O_EXCL, 0644, value);
  assert(this->semaphore != SEM_FAILED);
#else
  ::sem_init(&this->semaphore, 0 /*pshared*/, value);
#endif
}

Semaphore::~Semaphore() {
#if USE_NAMED_SEMAPHORE
  ::sem_close(this->semaphore);
  ::sem_unlink(this->name);
#else
  ::sem_destroy(&this->semaphore);
#endif
}

void Semaphore::signal() {
#if USE_NAMED_SEMAPHORE
  ::sem_post(this->semaphore);
#else
  ::sem_post(&this->semaphore);
#endif
}

void Semaphore::wait() {
#if USE_NAMED_SEMAPHORE
  ::sem_wait(this->semaphore);
#else
  ::sem_wait(&this->semaphore);
#endif
}

bool Semaphore::try_wait() {
  #if USE_NAMED_SEMAPHORE
  return ::sem_trywait(this->semaphore) == 0;
#else
  return ::sem_trywait(&this->semaphore) == 0;
#endif
}
