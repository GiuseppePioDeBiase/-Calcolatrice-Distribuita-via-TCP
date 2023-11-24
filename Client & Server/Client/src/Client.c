/*
 ============================================================================
 Name        : Client.c
 Author      : Giuseppe Pio de Biase
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

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

/* Prototype */
int communication(int socketClient);
void exitProgram(int socketClient);
void sendOperands(int socketClient, int operand1, int operand2);
int getResult(int c_socket);


/* CODE */
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

    // create client socket
    int socketClient;
    socketClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketClient < 0) {
    	errorhandler("socket creation failed.\n");
    			closesocket(socketClient);
    			clearwinsock();
    			return -1;
    }

    // set connection settings
    struct sockaddr_in sad;
    sad.sin_family = AF_INET;
    sad.sin_addr.s_addr = inet_addr(PROTO_ADDRESS);// IP del server
    sad.sin_port = htons(PROTO_PORT); // Server port

    // connection
    if (connect(socketClient, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
    	errorhandler("Failed to connect.\n");
    	closesocket(socketClient);
    		clearwinsock();
    		return -1;
    }



    	while (1){
        if (communication(socketClient) != 0)
            break;
    	}
    return 0;
}


char getOperation(char Operation) {
	char operation[100], inputString[100];
	int check;
	// save operation in inputString
	inputString[0] = Operation;

	do {
		switch (inputString[0]) {
		   case '+':
			 strcpy(operation, "add");
			 check = 1;
		     break;

		   case 'x':
			strcpy(operation, "mult");
			check = 1;
		    break;

		   case '/':
			strcpy(operation, "division");
			check = 1;
		    break;

		   case '-':
			strcpy(operation, "sub");
			check = 1;
		    break;

		   case '=':
			strcpy(operation, "=");
			check = 1;
		    break;

		   default:
			printf("error!!!. Please enter a valid character (+ - x /): ");
			scanf("%c", &inputString[0]);
			check = 0;
		    break;
		};
	} while(check != 1);

	return operation[0];
}


int communication(int socketClient) {
    char operation[100], inputString[100];
    int operand1, operand2;
    int flag = 0;

    // Until user will insert = calculator will work
    do {

        printf("Insert an operation [es: + 23 45]: ");
    	fgets(inputString, sizeof(inputString), stdin);
    	sscanf(inputString, "%c %d %d", &operation, &operand1, &operand2);

    	printf("Sono qui 1");

    	//atoi convert the string(operation) in int
    	int result = atoi(operation);
    	if (result != 0 || (result == 0 && operation[0] == '0')) {
    		printf("error!!!. Please enter a valid character (+ - x /): \n");
    	} else {

			operation[0] = getOperation(operation[0]);


			printf("Sono qui 2");

			int stringLen = strlen(operation);
			if (send(socketClient, operation, stringLen, 0) != stringLen) {
				printf("Message too large\n");
				closesocket(socketClient);
				clearwinsock();
				flag = 1;
				return -1;
			}


			printf("Sono qui 3");

			int RecvByte,countByte = 0;
			char buf[512];

			printf("Valore RecvByte prima del recv %d:", RecvByte);
			if ((RecvByte = recv(socketClient, buf, 1024 - 1, 0)) <= 0) {
							printf("Connection closed");
							closesocket(socketClient);
							clearwinsock();
							flag = 1;
							return -1;
						}

			printf("Valore RecvByte dopo del recv %d:", RecvByte);



			printf("Sono qui 4");



			printf("Sono qui 5");

			// Check the type of operation received from the server
			if ((strcmp(buf, "ADD") || strcmp(buf, "MULT") || strcmp(buf, "DIVISION") || strcmp(buf, "SUB"))) {
				sendOperands(socketClient, operand1, operand2);
			} else {
				printf("Error in operation");
				printf("terminate client process");
				exitProgram(socketClient);
				flag = 1;
				return -1;
			}

			printf("Sono qui 6");

			// Display the result received from the server and Exit the program and display the termination message
			printf("Result: %d\n\n", getResult(socketClient));

			}
    } while(flag != 1);

    return -1;
}



void sendOperands(int socketClient, int operand1, int operand2) {
	printf("Operand 1: %d\n", operand1);
	printf("Operand 2: %d\n", operand2);

	// save operands into msg
	msg msg;
    msg.a = operand1;
    msg.a = htonl(msg.a);
    msg.b = operand2;
    msg.b = htonl(msg.b);

    send(socketClient, &msg, sizeof(msg), 0);
}

void exitProgram(int socketClient) {
    // Close the connection
    closesocket(socketClient);
    clearwinsock();
}

int getResult(int c_socket) {
    int result = 0;
    recv(c_socket, &result, sizeof(result), 0);
    return ntohs(result);
}
