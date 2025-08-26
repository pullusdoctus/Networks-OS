# Interaction Details

This document details how clients can interact with forks given last week's [protocol](../01:20250812/protocol.md). Additionally, it details how forks can then interact with servers, according to the same [protocol](../01:20250812/protocol.md).

## Client-Fork Interaction

Clients never communicate with the server directly. Instead, they talk to a fork process that handles client requests. According to the list of client methods outlined in last week's [protocol](../01:20250812/protocol.md), the following list details how each client method is dealt with by the fork, which are handled as terminal commands clientside:

- `request <ASCII-art-filename>`: The client requests a specific ASCII art from the fork. The fork must validate the filename, and then fetch that filename in the server. The server will reply accordingly, and the fork will relay that answer to the client. If the server replies a success method (`ok <contents>`), the fork will reply with the contents of the file; that is, the ASCII art. Otherwise, the fork will reply with an error message that ultimately comes from the server.
- `submit <new-file>`: The client submits a new file to be stored in the server. The fork must validate the file; the file's contents must be ASCII-only characters, and it must not exceed a filesize limit. If the file is valid, the fork will store its contents in a new file serverside. The server will reply with a status message that will be relayed to the client by the fork.
- `list`: The client asks for a list of all files currently stored in the server's disk. The fork will relay this request to the server, and the server will reply with the list of files, which will itself be relayed to the client by the fork.
- `info <ASCII-art-filename>`: The client requests a specific file's metadata. Like in `request`, the fork must validate the filename, and then fetch that file's info from the server. The server will reply with the file's metadata, and that information will be relayed to the client by the fork.
- `quit`: The client requests to end the connection. Fork closes the client socket, and does this by sending a `close` method to the server.

## Fork-Server Interaction

Since forks server as middlemen between clients and servers, their job is to validate client methods, and send the corresponding request to the server. Likewise, they must relay the server's reply to the client. The following table details how each fork method interacts with server methods, both of these outlined in last week's [protocol](../01:20250812/protocol.md):

| Client Method | Fork Method | Server Method |
|---|---|---|
| `request <ASCII-art-filename>` | Validate the requested filename, then forward that request to the server with `fetch <ASCII-art-filename>`. | The server looks for the file requested by the fork, and replies accordingly: `ok <ASCII-art>` if it finds the file and opens it succesfully; `err <error-message>` if something goes wrong. |
| `submit <new-file>` | Validate the file's contents, then forward those contents to the server with a storage request with `store <new-file-contents>`. | The server will create a new file and it will store the contents sent by the fork in it. It will return `ok <new-file-name>` on success, or `err <error-message>` otherwise. |
| `list` | The fork only relays this request to the server with `fetch-list`. | The server gets the list of files in disk, and sends them back to the fork with `ok <file-list>`. If something goes wrong, `err <error-message>` is used instead. |
| `info <ASCII-art-filename>` | Validate the requested filename, then forward that request to the server with `fetch-info <ASCII-art-filename>`. | The server gets the metadata for the specified file, and returns it to the fork with `ok <metadata>`. Again, `err <error-message>` is returned on error. |
| `quit` | Closes the client socket with `close <client-socket`. This method also notifies the server. | Theoretically, the server doesn't need to close anything, since client connections are handled by forks. Thus, the server only should return `ok <close>`. |

For all those cases where the fork must relay a server response to the client, the `reply <answer>` method is used, which, in essence, just takes whatever answer the server sent, and sends that to the client.
