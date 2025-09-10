// Copyright 2020-2024 Jeisson Hidalgo-Cespedes. ECCI-UCR. CC BY 4.0

#include <cassert>
#include <cstdlib>

#include "Thread.hpp"

Thread::Thread() {
}

Thread::~Thread() {
  delete this->thread;
}

int Thread::startThread() {
  assert(this->thread == nullptr);
  this->thread = new std::thread(&Thread::run, this);
  return EXIT_SUCCESS;
}

int Thread::waitToFinish() {
  assert(this->thread);
  this->thread->join();

  delete this->thread;
  this->thread = nullptr;

  return EXIT_SUCCESS;
}
