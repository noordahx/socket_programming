import time
from socket import *

DEFAULT_SERVER_NAME = '127.0.0.1'
DEFAULT_SERVER_PORT = 16111
BUFFER_SIZE = 4096

clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((DEFAULT_SERVER_NAME, DEFAULT_SERVER_PORT))

while True:
    command = input('Enter a command (POST, GET, DELETE, QUIT):').strip().upper()

    if command == 'POST':
        clientSocket.send(command.encode())
        print("Enter your message (end with '#'):")
        while True:
            message = input()
            if message == '#':
                clientSocket.send(message.encode())
                break
            clientSocket.send(message.encode())
    
    elif command == 'GET':
        clientSocket.send(command.encode())
        while True:
            modifiedSentence = clientSocket.recv(BUFFER_SIZE).decode()
            if modifiedSentence == '#':
                break
            print('From Server:', modifiedSentence)
    
    elif command == 'DELETE':
        clientSocket.send(command.encode())
        print("Enter message IDs to delete (end with '#'):")
        while True:
            message_id = input()
            if message_id == '#':
                clientSocket.send(message_id.encode())
                break
            clientSocket.send(message_id.encode())

    elif command == 'QUIT':
        clientSocket.send(command.encode())
        break

    else:
        print("ERROR - Command not understood")

clientSocket.close()
              