#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <locale.h>
//Se debe agregar al compilador la librería Ws2_32

#define DEFAULT_PORT "5001"
#define DEFAULT_BUFLEN 512

void vaciarString(char* str);
int numAleatorio(int min, int max);
char consonanteAleatoria();
char vocalAleatoria();
char caracterAleatorio();
void generarUsuario(int longitud, char* bufferUsuario);
void generarPassword(int longitud, char* bufferPassword);
void splitMensaje(char* mensaje, char* opcion, int* longitud);


int main() {
    setlocale(LC_CTYPE, "Spanish"); //Esto permite imprimir correcatemente caracteres con tilde y especiales

    srand(time(NULL));

    WSADATA wsaData;
    int iResult;

    // Inicializar Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup falló con el código: %d\n", iResult);
        return 1;
    }

    // Configurar socket para escuchar conexiones
    struct addrinfo *result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolver la dirección del servidor y puerto
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo falló con el código: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Crear el socket
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error al crear el socket: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Vincular el socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind falló con el código: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    // Escuchar conexiones entrantes
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen falló con el código: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Esperando conexiones...\n");
    // Aceptar una conexión de cliente
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept falló con el código: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else{
        printf("Conexión establecida.");
    }

    // Recibir y enviar datos hasta que el cliente cierre la conexión
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    char respuesta[DEFAULT_BUFLEN];
    int mantenerConexion = 1;
    int longitud;
    char opcion;

    do {
        vaciarString(recvbuf);
        vaciarString(respuesta);

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("\nSe recibieron %d bytes: %s\n", iResult, recvbuf);


            splitMensaje(recvbuf, &opcion, &longitud);

            if (opcion == 'u'){
                if (longitud < 5 || longitud > 15){
                    strcpy(respuesta, "ERROR. La longitud del nombre de usuario debe ser entre 5 y 15.");
                }
                else{
                    generarUsuario(longitud, respuesta);
                }
            }
            else if (opcion == 'c'){
                if (longitud < 8 || longitud >= 50){
                    strcpy(respuesta, "ERROR. La longitud de la contraseña debe ser entre 8 y 49.");
                }
                else{
                    generarPassword(longitud, respuesta);
                }
            }
            else if (opcion == 's'){
                strcpy(respuesta, "END");
                mantenerConexion = 0;
            }
            else{
                strcpy(respuesta, "ERROR. Opción incorrecta.");
            }

            // Enviar respuesta
            int iSendResult = send(ClientSocket, respuesta, (int)strlen(respuesta), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send falló con el código: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("\nSe enviaron %d bytes: %s\n", iSendResult, respuesta);
        }

        if (iResult == 0 || opcion == 's'){
            printf("\nConexión cerrada.\n");
        }

        if (iResult < 0){
            printf("recv falló con el código: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult >= 0 && mantenerConexion);

    // Cerrar el socket de cliente
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

void vaciarString(char* str){
    for (int i = 0; i < DEFAULT_BUFLEN; i++){
        str[i] = '\0';
    }
};

int numAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

char vocalAleatoria(){
    char vocales[] = "aeiou";
    int n = numAleatorio(0, strlen(vocales) - 1);

    return vocales[n];
};

char consonanteAleatoria() {
    char consonantes[] = "bcdfghjklmnpqrstvwxyz";
    int n = numAleatorio(0, strlen(consonantes)-1);

    return consonantes[n];
}

char caracterAleatorio(){
    char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int n = numAleatorio(0, strlen(caracteres)-1);

    return caracteres[n];
}

void generarUsuario(int longitud, char* bufferUsuario){
    int vocalOConsonante = 1;

    if (numAleatorio(0,1) == 0){
        vocalOConsonante = -1;
    }

    vaciarString(bufferUsuario);

    for (int i = 0; i < DEFAULT_BUFLEN; i++){
        if (i < longitud){
            if (vocalOConsonante > 0){
                bufferUsuario[i] = vocalAleatoria();
            }
            else {
                bufferUsuario[i] = consonanteAleatoria();
            }
        }
        else{
            bufferUsuario[i] = '\0';
        }
        vocalOConsonante = vocalOConsonante * (-1);
    }

};

void generarPassword(int longitud, char* bufferPassword){
    vaciarString(bufferPassword);

    for (int i = 0; i < DEFAULT_BUFLEN; i++){
        if (i < longitud){
            bufferPassword[i] = caracterAleatorio();
        }
        else{
            bufferPassword[i] = '\0';
        }
    }
};

void splitMensaje(char* mensaje, char* opcion, int* longitud){
    *opcion = mensaje[0];
    mensaje[0] = '0';
    *longitud = atoi(mensaje);
};
