import socket
from http.server import HTTPServer, SimpleHTTPRequestHandler

class MyHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/ip':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            self.wfile.write(f'Your IP address is {self.client_address[0]}'.encode())
            return
        else:
            return SimpleHTTPRequestHandler.do_GET(self)

class HTTPServerV6(HTTPServer):
    address_family = socket.AF_INET6

def main():
    server = HTTPServerV6(('::', 8080), MyHandler)
    server.serve_forever()

if __name__ == '__main__':
    main()
