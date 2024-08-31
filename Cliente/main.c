#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <locale.h>
//Se debe agregar al compilador la librería Ws2_32

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "5001"
#define DEFAULT_SERVER "127.0.0.1"

void vaciarString(char* str);
void menu(char* sendbuf);


int main(void) {
    setlocale(LC_CTYPE, "Spanish"); //Esto permite imprimir correcatemente caracteres con tilde y especiales


    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char sendbuf[DEFAULT_BUFLEN];
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;


    // Inicializar Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup ha fallado. Codigo de error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolver la dirección del servidor y el puerto
    iResult = getaddrinfo(DEFAULT_SERVER, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo ha fallado. Codigo de error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Intentar conectarse al servidor
    printf("Intentando establecer conexón con el servidor...\n");

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket ha fallado. Codigo de error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("No se pudo conectar al servidor.\n");
        WSACleanup();
        return 1;
    }
    else{
        printf("Conexión establecida.\n");
    }

    // Enviar y recibir mensajes hasta salir
    do{
        // Enviar mensaje
        vaciarString(sendbuf);
        menu(sendbuf);

        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send ha fallado. Codigo de error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }
        //printf("Se enviaron %d bytes: %s\n", iResult, sendbuf);


        // Recibir una respuesta del servidor
        vaciarString(recvbuf);
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0 && (strcmp(recvbuf, "END") != 0)) {
            //printf("Bytes recibidos: %d\n", iResult);
            printf("Mensaje del servidor: %s\n", recvbuf);
        } else if (iResult < 0){
            printf("recv ha fallado. Codigo de error: %d\n", WSAGetLastError());
        } else {
            printf("Conexión cerrada.\n");
        }
    } while((iResult >= 0) && (strcmp(recvbuf, "END") != 0));

    // Cerrar la conexión
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

void vaciarString(char* str){
    for (int i = 0; i < DEFAULT_BUFLEN; i++){
        str[i] = '\0';
    }
};

void menu(char* sendbuf){
    int menuOpcion;
    char opcion;
    int longitud;

    do{
        menuOpcion = 0;
        longitud = 0;

        printf("\nElegí una opción del menú:\n");
        printf("1 - Generar nombre de usuario\n");
        printf("2 - Generar contraseña\n");
        printf("3 - Salir\n");
        printf("> ");

        fflush(stdin);
        scanf("%d", &menuOpcion);

        if (menuOpcion == 1) {opcion = 'u';}
        if (menuOpcion == 2) {opcion = 'c';}
        if (menuOpcion == 3) {opcion = 's';}

        if (menuOpcion == 1 || menuOpcion == 2){
            printf("\nIngresá la longitud deseada:\n> ");

            fflush(stdin);
            scanf("%d", &longitud);
        }
        else if (menuOpcion == 3){
            printf("Saliendo...\n");
        }
        else{
            printf("Opción incorrecta.\n");
        }
    } while (menuOpcion < 1 || menuOpcion > 3);

    sprintf(sendbuf, "%c%d", opcion, longitud);
};
