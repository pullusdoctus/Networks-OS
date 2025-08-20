# Protocol Prototype

Since the project aims to retrieve ASCII art stored in `txt` files, this initial protocol proposes a series of potentially useful methods for this purpose:

## Client Methods

- `request <ASCII-art-filename>`: Requests an ASCII art by name.
- `submit <new-file>`: Submits a file to be stored in the server.
- `list`: Lists all ASCII art in storage.
- `info <ASCII-art-filename>`: Requests information about a specific file, like filesize, line count, character count, etc.
- `quit`: Ends the connection with the server.

## Server Methods

- `ok`: Lets the client know that the request was handled appropriately. Depending on the client's request, the `ok` method is accompanied by its appropriate response (e.g.: a client's `fetch` is replied with `ok` followed by the ASCII art).
- `err`: Lets the client know an error occured. It's followed by a descriptive message of the error.
- `close <client-socket>`: Closes the connection with a client's socket.
- `shutdown`: Lets all clients know the server is shutting down. Sends a message letting all clients the server is closing, closes all client connections, and then ends execution.

## Fork Methods

- `fetch <ASCII-art-filename>`: Fetches a specific ASCII art in storage. This is the follow up to the `request` client method.
- `store <new-file>`: Tells the server to store a file in storage. The fork will send this request once it has verified the file's contents are non-malicious, and they're handleable by the server (i.e., the file contains ASCII art). This is the follow up to the `submit` client method.
- `fetch-list`: Gets the list of all files in storage. Returns that list to the client.
- `fetch-info <ASCII-art-filename`: Gets information about a specific file, like filesize, line count, character count, etc.
- `reply <answer>`: Sends the answer to the client.
- `close <client-socket>`: Tells the server to close a connection with a client.

# Client-Server Simulation

The following are some examples of how this protocol would be used. Each oval represents a thread. Each arrow represents a message. Thus, clients send messages to forks, and they receive from forks; forks receive and send messages to both clients and servers; and servers receive messages from forks, and send to forks. This means that if a thread hasn't received a message, it is constantly waiting for one. The moment it receives a message, it acts accordingly.

All examples here follow an ideal program execution where it works without issues. Since the client takes the initial step by sending the initial request, assume that the client's message sent to the fork starts it all.

## Request Method Example

![request method example](./img/request.svg)

## Submit Method Example

![submit method example](./img/submit.svg)

## List Method Example

![list method example](./img/list.svg)

## Info Method Example

![info method example](./img/info.svg)

## Quit Method Example

![quit method example](./img/quit.svg)
