// Copyright 2021 Jeisson Hidalgo-Cespedes. Universidad de Costa Rica. CC BY 4.0

#ifndef COMMON_HPP
#define COMMON_HPP

/// Declare four of the rule-of-the-five methods
#define DECLARE_RULE4(Class, action) \
  Class(const Class& other) = action; \
  Class(Class&& other) = action; \
  Class& operator=(const Class& other) = action; \
  Class& operator=(Class&& other) = action

/// Disable default methods for copying objects
#define DISABLE_COPY(Class) \
  DECLARE_RULE4(Class, delete)

/// For data transmissions
typedef unsigned char byte;

#endif  // COMMON_HPP