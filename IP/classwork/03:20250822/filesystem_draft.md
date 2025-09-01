# Filesystem Draft

This document summarizes the current filesystem class design. The following UML diagram illustrates the draft model:

![filesystem draft](./img/filesystem_draft.svg)

---

## Classes

### **FilesystemMetadata**

Represents the whole filesystem, storing global information and providing methods to manage block allocation.

**Attributes**

* `blockSize: int`: size of each block in bytes (or kilobytes).
* `blockCount: int`: total number of blocks in the filesystem.
* `freeBlockCount: int`: number of blocks currently available.
* `blockCapacity: int`: maximum storage capacity based on block size and count.
* `root: Node`: reference to the root directory node.

**Methods**

* `allocateBlock(): int`: reserves and returns the ID of a free block.
* `freeBlock(blockId: int): int`: releases a block for reuse.
* `mount()`: initializes the filesystem.
* `unmount()`: safely shuts down the filesystem.

---

### **Node**

Represents a generic file system entry (either a file or a directory).

**Attributes**

* `parent: Node`: reference to the parent directory.
* `blocks: DoublyLinkedList<int>`: collection of block pointers where the node’s data is stored.
* `id: int`: This Node's ID.

**Methods**

* `getPath(): str`: returns the absolute path to this Node from the Root.
* `getSize(): int`: returns this Node's size in bytes (or kilobytes).
* `grow(blockId: int): void`: grows the Node by inserting a new block.
* `shrink(blockId: int): void`: shrinks the Node by removing one of its blocks.
* `delete(): void`: deletes the Node's data. Removes it from the filesystem.
* `abstract isDir(): bool`: `true` if the Node is a Directory; `false` otherwise.

---

### **File** (inherits from Node)

Represents a regular file, storing metadata specific to files.

**Attributes**

* `filename: str`: the file’s name.
* `extension: str`: the file’s extension.
* `size: int`: the size of the file in bytes.
* `permissions: int[]`: array of permissions a file may have.

**Methods**

* `read(offset: int, length: int): str`: reads `length` bytes from the File starting at `offset`, and returns the data as a string.
* `write(offset: int, data: str): int`: writes `data` to the File starting at `offset`, and returns the number of bytes written.
* `append(data: str): int`: appends `data` to the end of the File, and returns the number of bytes written.

---

### **Directory** (inherits from Node)

Represents a directory, modeled as a mapping of names to nodes (files or subdirectories).

**Attributes**

* `children: Map<id, Node>`: dictionary mapping names to nodes within the directory.

**Methods**

* `addChild(child: Node): void`: adds a child Node to the Directory.
* `removeChild(childId: int): void`: removes a child Node from the Directory, indexed by its hash ID.
* `isEmpty(): bool`: `true` if the directory is empty; `false` otherwise.
* `moveChild(childId: int, newParent: Node): void`: moves a child, indexed by its hash ID, to another directory, which is `newParent`.
