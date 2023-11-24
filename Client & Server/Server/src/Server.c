/*
 ============================================================================
 Name        : Server.c
 Author      : Giuseppe_Pio_de_Biase
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "protocol.h"

#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "protocol.h"

void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

void errorhandler(char *errorMessage) {
	printf("%s", errorMessage);
}

// Prototype
int communication(int c_socket, int mySocket);
void Door_IP_Detector(int c_socket, int mySocket);
void exitProgram(int mySocket);
int sendMessage(int c_socket, int mySocket, char *inputServer);
msg getOperands(int c_socket, int mySocket);
int performOperation(int a, int b, char operation);
void sendResult(int socketClient, int result);

// Math operations functions
int add(int a, int b);
int mult(int a, int b);
int sub(int a, int b);
int division(int a, int b);

int main(int argc, char *argv[]) {
#if defined WIN32
	// Initialize Winsock
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		printf("Error at WSAStartup()\n");
		return 0;
	}
#endif

	// create welcome socket
		int mySocket;
		mySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mySocket < 0) {
			errorhandler("socket creation failed.\n");
			clearwinsock();
			return -1;
		}else{
			printf("the socket creation was successful\n");
		}

    // BIND FUNCTION
		 struct sockaddr_in sad;
		    sad.sin_family = AF_INET; //memset crashes the server
		    sad.sin_addr.s_addr = inet_addr(PROTO_ADDRESS);
		    sad.sin_port = htons(PROTO_PORT);

		    if (bind(mySocket, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
		        puts("Binding failed!\n");
		        closesocket(mySocket);
		        clearwinsock();
		        return -1;
		    } else {
		    	printf("Binding created was successful!\n");
		    }


			// listen

			if (listen(mySocket, QLEN) < 0) {
				errorhandler("listen() failed.\n");
				closesocket(mySocket);
				clearwinsock();
				return -1;
			}


	// accept new connection
    struct sockaddr_in cad;
    int c_socket;
    int client_len;          // the size of the client address
    printf("Waiting for a client to connect...\n\n");
    while (1) {
        client_len = sizeof(cad);
        if ((c_socket = accept(mySocket, (struct sockaddr*) &cad, &client_len)) < 0) {
        			errorhandler("accept() failed.\n");
        			// close connection
        			closesocket(c_socket);
        			clearwinsock();
        			return 0;
        }

        Door_IP_Detector(c_socket, mySocket);

        while (1) {				//continuous communication between the server and the client
            if (communication(c_socket, mySocket) != 0)
                break;
        }
    }
}// main end

int communication(int c_socket, int mySocket) {
    char buf[512];

    if ((recv(c_socket, buf, 1024 - 1, 0)) <= 0) {
        printf("Connection interrupted");
        return -1;
    }

    char *inputServer = "Connection established";
    int stringLen = strlen(inputServer);
    send(c_socket, inputServer, stringLen, 0);
    puts("");

    msg operands = getOperands(c_socket, mySocket);

    int result = performOperation(operands.a, operands.b, buf[0]);
    result = htons(result);

    send(c_socket, &result, sizeof(result), 0);
    return 0;
}

msg getOperands(int c_socket, int mySocket) {
	msg msg;
    recv(c_socket, (char *)&msg, sizeof(msg), 0);
    msg.a = ntohl(msg.a);
    msg.b = ntohl(msg.b);
    return msg;
}

int sendMessage(int c_socket, int mySocket, char *inputServer) {
    // SEND MESSAGE FUNCTION
    int stringLen = strlen(inputServer);
    if (send(c_socket, inputServer, stringLen, 0) != stringLen) {
        puts("Message too large");
        system("pause");
        closesocket(mySocket);
        clearwinsock();
        return -1;
    }
    if (strcmp(inputServer, "exit") == 0) {
        exitProgram(mySocket);
    }
    puts("");
    // ------------------------------------------------------------
    return 0;
}

void  Door_IP_Detector(int c_socket, int mySocket) {
    // Utilizza le costanti definite in protocol.h per ottenere l'indirizzo IP e la porta
    char* clientIP = PROTO_ADDRESS;
    int clientPort = PROTO_PORT;

    // Mostra l'indirizzo IP e il numero di porta del client
    printf("Connection established with %s:%d\n", clientIP, clientPort);

    // Invia un messaggio di connessione riuscita al client

}

void exitProgram(int mySocket) {
    // CLOSE THE CONNECTION
    closesocket(mySocket);
    clearwinsock();
}

// Addition function
int add(int a, int b) {
    return a + b;
}

// Multiplication function
int mult(int a, int b) {
    return a * b;
}

// Subtraction function
int sub(int a, int b) {
    return a - b;
}


// Division function
int division(int a, int b) {
    if (b == 0) {
        return 0;
    } else {
        return a / b;
    }
}
int performOperation(int a, int b, char operation) {
    switch (operation) {
        case 'a':
            return add(a, b);
        case 'm':
            return mult(a, b);
        case 'd':
            return division(a, b);
        case 's':
            return sub(a, b);
        default:
            return -1;
    }
}


