# Classwork for September 2nd, 2025

## Proof of Working Cables

![connection tester with working results](./cable.jpeg)

## Protocol

The protocol agreed upon can be found in [here](./protocol.pdf).

## File System

The File System now aims to be indexed. The class structure can be seen in the following UML:

![filesystem UML](./filesystem.svg)

## FilesystemMetadata

Represents the File System. It stores global information and manages blocks and indeces.

### Attributes

- `blockSize: int`: block size in bytes. By default, should be 256.
- `blockCount: int`: total number of blocks in the File System.
- `freeBlockCount: int`: number of available blocks.
- `blockCapacity: int`: maximum storage capacity based on block size and block count.
- `root: Directory`: reference to the root directory.

### Methods

- `allocateBlock(): int`: reserves and returns the ID of a free block.
- `freeBlock(blockId: int): void`: frees a block for reuse.
- `allocateIndex(): int`: allocates a block to be used as an index block.
- `freeIndex(blockId: int): void`: frees an index block.
- `mount()`: mounts the File System's root and initializes the File System.
- `unmount()`: safely shuts down the File System.

## Node

A generic file system node (inode).

### Attributes

- `parent: Node`: reference to the parent directory.
- `id: int`: This Node's ID.
- `index: int`: block ID of this Node's index block.

### Methods

- `getPath(): str`: returns the absolute path to this Node from the Root.
- `getSize(): int`: returns this Node's size in bytes.
- `grow(): void`: allocates a new data block and updates this Node's index block.
- `shrink(): void`: frees a data block and updates this Node's index block.
- `delete(): void`: deletes the Node's data and index blocks and removes them from the File System.
- `abstract isDir(): bool`: `true` if the Node is a Directory; `false` otherwise.

## Node::File

A regular file.

### Attributes

- `filename: str`: the file's name.
- `extension: str`: the file's extension.
- `size: int`: the file's size in bytes.
- `permissions: int[]`: array of permissions a file may have.

### Methods

- `read(offset: int, length: int): str`: reads `lenght` bytes from the File starting at `offset`, using the index block to find the data blocks.
- `write(offset: int, data: str): int`: writes `data` to the File starting at `offset`, using the index block to find the data blocks, and returns the number of bytes written.
- `append(data: str): int`: appends `data` to the end of the file, allocating new blocks if needed, and returns the number of bytes written.

## Node::Directory

A directory.

### Attributes

- `children: Map<id, Node>`: maps Node IDs to the actual Nodes in the Directory.

### Methods

- `addChild(child: Node): void`: adds a child Node to the Directory and updates its index block.
- `removeChild(childId: int): void`: removes a childe Node from the Directory and updates its index black.
- `isEmpty(): bool`: `true` if the directory is empty; `false` otherwise.
- `moveChild(childId: int, newParent: Node): void`: moves a child by its Node ID to another Directory.
