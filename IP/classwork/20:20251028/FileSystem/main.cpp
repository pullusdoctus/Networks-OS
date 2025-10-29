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

static bool expectTrue(const std::string& testName, bool condition,
                       const std::string& msg = "") {
  std::cout << (condition ? "[PASS] " : "[FAIL] ") << testName;
  if (!msg.empty()) std::cout << ": " << msg;
  std::cout << "\n";
  return condition;
}

int main(int argc, char** argv) {
  bool createFresh = true;  // Default to fresh for consistent testing
  if (argc > 1 && std::string(argv[1]) == "existing") {
    createFresh = false;
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

  // Test F: Write exactly one block
  header("Test F: single block write");
  fs->crearInodo("oneblock.txt");
  std::string oneBlock(TBLOQUE - 1, 'X');
  ok = fs->escribir("oneblock.txt", oneBlock.c_str());
  r = fs->leer("oneblock.txt", TBLOQUE);
  if (expectEqual("single block read", r, oneBlock)) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test G: Write exactly two blocks
  header("Test G: two block write");
  fs->eliminar("twoblock.txt");  // Clean up
  fs->crearInodo("twoblock.txt");
  std::string twoBlocks = std::string(TBLOQUE, 'Y') + std::string(TBLOQUE, 'Z');
  std::cout << "[DEBUG] Input string: first char='" << twoBlocks[0] 
            << "' char at 255='" << twoBlocks[255]
            << "' char at 256='" << twoBlocks[256]
            << "' last='" << twoBlocks[511] << "'\n";
  std::cout << "[DEBUG] Writing " << twoBlocks.size() << " bytes\n";
  ok = fs->escribir("twoblock.txt", twoBlocks.c_str());
  std::cout << "[DEBUG] Write " << (ok ? "succeeded" : "failed") << "\n";
  
  // Read back and check
  r = fs->leer("twoblock.txt", TBLOQUE * 2 + 10);
  if (r) {
    int readLen = static_cast<int>(strlen(r));
    std::cout << "[DEBUG] Read " << readLen << " bytes (expected 512)\n";
    std::cout << "[DEBUG] Read string: first char='" << r[0] 
              << "' char at 255='" << r[255]
              << "' char at 256='" << (readLen > 256 ? r[256] : '?')
              << "'\n";
    if (std::string(r) == twoBlocks) {
      ++passed;
      std::cout << "[PASS] two block read matches\n";
    } else {
      ++failed;
      std::cout << "[FAIL] two block read mismatch\n";
    }
  } else {
    ++failed;
    std::cout << "[FAIL] two block read returned null\n";
  }
  delete[] r;

  // Test H: Overwrite existing file with smaller content
  header("Test H: overwrite with smaller content");
  ok = fs->escribir("twoblock.txt", "small");
  r = fs->leer("twoblock.txt", 32);
  if (expectEqual("overwrite read", r, "small")) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test I: Multiple appends to empty file
  header("Test I: multiple appends from empty");
  fs->eliminar("multi.txt");  // Clean up if exists
  fs->crearInodo("multi.txt");
  std::cout << "[DEBUG] Created empty file, now appending 10 chars\n";
  for (int i = 0; i < 10; ++i) {
    bool appendOk = fs->agregar("multi.txt", 'A' + i);
    if (!appendOk) {
      std::cout << "[DEBUG] Append " << i << " failed\n";
    }
  }
  r = fs->leer("multi.txt", 32);
  if (r) {
    std::cout << "[DEBUG] Read result: [" << r << "] (length " 
              << strlen(r) << ")\n";
  }
  if (expectEqual("multi append", r, "ABCDEFGHIJ")) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test J: Delete non-existent file
  header("Test J: delete non-existent file");
  ok = fs->eliminar("nonexistent.txt");
  if (expectTrue("delete non-existent", !ok, "should return false")) {
    ++passed;
  } else {
    ++failed;
  }

  // Test K: Rename non-existent file
  header("Test K: rename non-existent file");
  ok = fs->renombrar("ghost.txt", "phantom.txt");
  if (expectTrue("rename non-existent", !ok, "should return false")) {
    ++passed;
  } else {
    ++failed;
  }

  // Test L: Create file with maximum name length
  header("Test L: maximum filename length");
  fs->crearInodo("verylongname123");  // 15 chars (max allowed)
  ok = fs->escribir("verylongname123", "maxname");
  r = fs->leer("verylongname123", 16);
  if (expectEqual("max name read", r, "maxname")) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test M: Read partial content
  header("Test M: partial read");
  fs->eliminar("append.txt");  // Free space
  fs->crearInodo("partial.txt");
  fs->escribir("partial.txt", "0123456789ABCDEFGHIJ");
  r = fs->leer("partial.txt", 10);
  if (expectEqual("partial read", r, "0123456789")) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test N: Empty file write and read
  header("Test N: empty content");
  fs->eliminar("empty.txt");  // Clean up
  fs->crearInodo("empty.txt");
  // Don't write anything - just test reading empty file
  r = fs->leer("empty.txt", 10);
  if (expectNotExists("empty file read", r)) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test O: Large file spanning indirect blocks
  header("Test O: large file with indirect blocks");
  fs->eliminar("oneblock.txt");  // Free up space
  fs->eliminar("partial.txt");   // Free up space
  fs->crearInodo("large.txt");
  std::string largeContent;
  for (int i = 0; i < 1500; ++i) {
    largeContent += static_cast<char>('A' + (i % 26));
  }
  ok = fs->escribir("large.txt", largeContent.c_str());
  r = fs->leer("large.txt", 2000);
  if (r && std::string(r) == largeContent) {
    ++passed;
    std::cout << "[PASS] large file matches (" << largeContent.size() 
              << " bytes)\n";
  } else {
    ++failed;
    std::cout << "[FAIL] large file mismatch\n";
  }
  delete[] r;

  // Test P: Modify existing file
  header("Test P: modify file");
  ok = fs->modificar("c.txt", "modified_content");
  r = fs->leer("c.txt", 32);
  if (expectEqual("modify read", r, "modified_content")) {
    ++passed;
  } else {
    ++failed;
  }
  delete[] r;

  // Test Q: Sequential writes and reads
  header("Test Q: sequential operations");
  fs->eliminar("verylongname123");  // Free space
  fs->eliminar("large.txt");        // Free space
  fs->eliminar("multi.txt");        // Free space
  for (int i = 0; i < 3; ++i) {
    std::string name = "seq" + std::to_string(i) + ".txt";
    std::string content = "content_" + std::to_string(i);
    fs->crearInodo(name);
    fs->escribir(name.c_str(), content.c_str());
  }
  bool allMatch = true;
  for (int i = 0; i < 3; ++i) {
    std::string name = "seq" + std::to_string(i) + ".txt";
    std::string expected = "content_" + std::to_string(i);
    r = fs->leer(name.c_str(), 32);
    if (!r || std::string(r) != expected) {
      allMatch = false;
      std::cout << "[FAIL] seq" << i << " mismatch\n";
    }
    delete[] r;
  }
  if (allMatch) {
    ++passed;
    std::cout << "[PASS] all sequential files match\n";
  } else {
    ++failed;
  }

  // Test R: Append across block boundary
  header("Test R: append across block boundary");
  fs->eliminar("seq0.txt");  // Free space
  fs->eliminar("seq1.txt");
  fs->eliminar("seq2.txt");
  fs->crearInodo("boundary.txt");
  std::string base(TBLOQUE - 5, 'B');
  fs->escribir("boundary.txt", base.c_str());
  for (int i = 0; i < 10; ++i) {
    fs->agregar("boundary.txt", 'X');
  }
  r = fs->leer("boundary.txt", TBLOQUE + 20);
  std::string expected = base + std::string(10, 'X');
  if (r && std::string(r) == expected) {
    ++passed;
    std::cout << "[PASS] boundary append works\n";
  } else {
    ++failed;
    std::cout << "[FAIL] boundary append failed\n";
  }
  delete[] r;

  // Summary and state
  header("Summary");
  std::cout << "Passed: " << passed << "  Failed: " << failed << "\n";
  std::cout << "Total tests: " << (passed + failed) << "\n";
  
  header("Final Directory State");
  fs->imprimirDirectorio();

  delete fs;
  return (failed == 0) ? 0 : 2;
}