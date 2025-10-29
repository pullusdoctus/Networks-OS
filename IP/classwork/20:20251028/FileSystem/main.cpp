#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// Copyright 2025 pullusdoctus

#include "FileSystem.h"

static void header(const std::string& title) {
  std::cout << "\n===== " << title << " =====\n";
}

static bool expectEqual(const std::string& testName, const char* got,
                        const std::string& want) {
  bool pass = (got != nullptr) && (strcmp(got, want.c_str()) == 0);
  std::cout << (pass ? "[PASS] " : "[FAIL] ") << testName << ": ";
  if (pass) {
    std::cout << "got expected value\n";
  } else {
    std::cout << "expected=[" << want << "] got=[" << (got ? got : "(null)")
              << "]\n";
  }
  return pass;
}

static bool expectNotExists(const std::string& testName, char* got) {
  bool pass = (got == nullptr);
  std::cout << (pass ? "[PASS] " : "[FAIL] ") << testName << ": ";
  if (pass) {
    std::cout << "file not found as expected\n";
  } else {
    std::cout << "expected missing file but read: [" << got << "]\n";
  }
  return pass;
}

int main(int argc, char** argv) {
  bool createFresh = false;
  if (argc > 1 && std::string(argv[1]) == "fresh") {
    createFresh = true;
  }

  header(std::string("FileSystem tests - ") +
         (createFresh ? "fresh create" : "load existing"));

  FileSystem* fs = new FileSystem(createFresh);

  int passed = 0, failed = 0;

  // Test A: create files and basic write/read
  header("Test A: create + write + read");
  fs->crearInodo("a.txt");
  fs->escribir("a.txt", "hola!");
  fs->crearInodo("b.txt");
  fs->escribir("b.txt", "bbb");
  fs->crearInodo("c.txt");
  fs->escribir("c.txt", "ccc");

  char* r = fs->leer("a.txt", 32);
  if (expectEqual("read a.txt", r, "hola!")) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test B: rename and read
  header("Test B: rename + read");
  bool ok = fs->renombrar("a.txt", "ballena.txt");
  std::cout << (ok ? "[INFO] rename succeeded\n" : "[WARN] rename failed\n");
  r = fs->leer("ballena.txt", 64);
  if (expectEqual("read renamed file", r, "hola!")) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test C: replace with larger content (exercise multi-block behavior)
  header("Test C: replace with larger content");
  const std::string big = "BALLENA_" + std::string(100, 'A');
  ok = fs->reemplazar("ballena.txt", big.c_str());
  std::cout << (ok ? "[INFO] replace succeeded\n" : "[WARN] replace failed\n");
  r = fs->leer("ballena.txt", static_cast<int>(big.size()) + 8);
  if (r && std::string(r) == big) {
    ++passed;
    std::cout << "[PASS] big read matches\n";
  } else {
    ++failed;
    std::cout << "[FAIL] big read mismatch or null\n";
  }
  delete[] r;

  // Test D: delete and verify missing
  header("Test D: delete + verify missing");
  ok = fs->eliminar("b.txt");
  std::cout << (ok ? "[INFO] delete b.txt succeeded\n"
                   : "[WARN] delete b.txt failed\n");
  r = fs->leer("b.txt", 16);
  if (expectNotExists("read deleted b.txt", r)) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test E: append many bytes to exercise allocation
  header("Test E: append to file (multi-block)");
  fs->eliminar("ballena.txt");
  fs->crearInodo("append.txt");
  fs->escribir("append.txt", "START");
  for (int i = 0; i < 100; ++i) fs->agregar("append.txt", (i % 10) + '0');
  int expectLen = 5 + 100;
  r = fs->leer("append.txt", expectLen + 4);
  if (r) {
    int gotLen = static_cast<int>(strlen(r));
    if (gotLen == expectLen) {
      ++passed;
      std::cout << "[PASS] append length matches\n";
    } else {
      ++failed;
      std::cout << "[FAIL] append length expected=" << expectLen
                << " got=" << gotLen << "\n";
    }
  } else {
    ++failed;
    std::cout << "[FAIL] append read returned null\n";
  }
  delete[] r;

  // Summary and state
  header("Summary");
  std::cout << "Passed: " << passed << "  Failed: " << failed << "\n\n";

  delete fs;
  return (failed == 0) ? 0 : 2;
}
