#include "../utils/config.h"

int main(int argc, char *argv[])
{
    printf("[i] Initializing Client\n");
    char *serverIp = getUserInput("Server IP Address: ", BUF_SIZE);

    SOCKET client_socket = createSocket(AF_INET, SOCK_STREAM, 0);
    initializeSocket(&client_socket, serverIp, PORT);
}
