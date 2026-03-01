#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5960 // Порт вашего сервера
#define HOST "127.0.0.1"

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[8192];
    char input[1024];

    // Создаём сокет
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Преобразуем IP-адрес
    if (inet_pton(AF_INET, HOST, &servaddr.sin_addr) <= 0) {
        perror("invalid address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Подключаемся к серверу
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    

    while (1) {
    
    memset(input,0,sizeof(input));
    if (fgets(input, sizeof(input), stdin) == NULL) break;

    // Убираем \n только для проверки команды выхода
    input[strcspn(input, "\n")] = 0;
    if (strcmp(input, "/q") == 0 || strcmp(input, "/quit") == 0) break;

    // Отправляем команду и разделитель
    write(sockfd, input, strlen(input));
    write(sockfd, "\n", 1);   // обязательно добавляем \n

    // Читаем ответ сервера
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = read(sockfd, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }
}

    close(sockfd);
    return 0;
}
