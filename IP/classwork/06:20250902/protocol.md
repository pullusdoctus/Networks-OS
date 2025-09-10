# Protocol Prototype II

This is the protocol agreed upon by the groups 'IPv3' and 'Monkis404'.

## Client Methods

- `list`: Asks for the list of files within the filesystem.
- `request`: Asks for an ASCII art found in a file specified as a parameter (e.g. `request whale-1.txt`).
- `submit`: Uploads a file to the server.
- `delete`: Deletes a file from the server.

## Fork Methods

- `fetchList`: Gets the list of all files in storage. Returns that list to the client.
- `fetchFile`: Fetches a specific ASCII art in storage. This is the follow up to the `request` client method.
- `store`: Tells the server to store a file in storage. The fork will send this request once it has recieved the file's contents from the client. This is the follow up to the `submit` client method.
- `remove`: Tells the server to delete a file from storage. This is the follow up to the `delete` client method.
- `connect`: Establishes a connection with a server.
- `kill`: Ends the connection it has with a server.

## Server Behavior

| Client Method | Fork Method | Server Action |
|---|---|---|
| `list` | `fetchList` | Traverses the File System (use `traverse` method for FS) looking for available files. |
| `request` | `fetchFile` | Searches the file in the File System (use `search` method for FS), opens it (use `open` method for FS), and returns its contents (use `read` method for FS) |
| `submit` | `store` | Receives the file's name and contents, creates a file with the correct name (use `create` method for FS), and writes its contents (use `write` method for FS) |
| `delete` | `remove` | Receives the filename and deletes it from the File System (use `delete` method from FS) |
| - | `connect` | Receives a connection from the fork and accepts it. Creates a socket for this |
| - | `kill` | Closes the socket with the fork |

## Credits

### Group 3: IPv3

- Alexa Alpízar C20281
- Leonardo Calderón C31452
- Jeremy Rojas B96804
- Adrián Arrieta B70734

### Group 4: Monkis404

- Geiner Montoya C25063
- Carlos Obando C35655
- Anthony Sanchez C37331
- Andrés Camacho C01544
