# socket programming library
from socket import *
# library needed to read args
import sys


# default values for address, port and message buffer size (max size of msg content)
DEFAULT_SERVER_NAME = '127.0.0.1'
DEFAULT_SERVER_PORT = 16111
BUFFER_SIZE = 4096


class MessageBoardClient():
    def __init__(self, server_name = DEFAULT_SERVER_NAME, 
                 server_port = DEFAULT_SERVER_PORT):
        # keep server address and port in our class variables
        self.server_name = server_name
        self.server_port = server_port

    def start_connection(self):
        # Socket Initialization (task 1)
        self.client_socket = socket(AF_INET, SOCK_STREAM)
        # Init connection (takes default values for name, port if not specified)
        self.client_socket.connect((self.server_name, self.server_port))

    def end_connection(self):
        # close socket (task 5)
        self.client_socket.close()

    def listen(self):
        # receive the message form the server (task 4)
        # we get the message first
        message = self.client_socket.recv(BUFFER_SIZE).decode()
        # print with server: prefix
        print(f"server: {message}")
        # return content of the message for later use in calling functions
        return message

    def handle_post(self):
        while True:
            message = input("client: ")
            if message == '#':
                self.client_socket.send(message.encode())
                self.listen()
                break
            self.client_socket.send(message.encode())

    def handle_get(self):
        # while server has message to deliver, keep listening
        while True:
            sentence = self.listen()
            # only if message content in #, stop listening
            if sentence == '#':
                break
    
    def handle_delete(self):
        # can delete multiple messages at once, hence using loop
        while True:
            id = input("client: ")
            self.client_socket.send(id.encode())
            # when done deleting, we receive '#' character, breaking our outer loop
            if id == '#':
                self.listen()
                break
    
    def start_app(self):
        self.start_connection()
        # program keeps running until QUIT command is hit
        while True:
            command = input('client: ').strip().upper()
            # Send command to the server (task 3)
            self.client_socket.send(command.encode())
            
            # Each command calls corresponding function
            if command == "POST":
                self.handle_post()
            elif command == "GET":
                self.handle_get()
            elif command == "DELETE":
                self.handle_delete()
            elif command == 'QUIT':
                self.listen()
                # only QUIT breaks the outer loop
                break
            else:
                self.listen()
        
        # Close socket (task 5)
        self.end_connection()
              
if __name__ == '__main__':
    n = len(sys.argv)
    if n == 3:
        # if custom server address and port is specified, use those
        client = MessageBoardClient(sys.argv[1], int(sys.argv[2]))
    else:
        # otherwise use defaults
        client = MessageBoardClient()

    # entry point of our class
    client.start_app()