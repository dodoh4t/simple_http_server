#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;    // sockaddr_in 구조체 로드 후 address 선언
    int opt = 1;    // SO_REUSE ADDR 및 SO_REUSEPORT 옵션을 설정하는데 사용되는 정수
    int addrlen = sizeof(address);    // addlen 의 경우 address 의 크기
    char buffer[1024] = {0};    // buffer 크기를 1024 로 만들고, 각 인덱스를 모두 0으로 초기화
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 18\n\nSimple HTTP Server";    // HTTP Response 내용 (200 OK)

    // Socket file descriptor 생성, 에러 발생 시 종료
    // AF_INET 인수는 소켓이 IPv4 프로토콜을 사용할 것임을 지정
    // SOCK_STREAM 인수는 소켓이 TCP 프로토콜을 사용할 것임을 지정
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 포트 바인딩, 에러 발생 시 종료
    // setsocket() 시스템 호출을 사용하여 소켓에 대한 SO_REUSE ADDR 및 SO_REUSEPORT 옵션 설정
    // address 구조체의 멤버 설정 (sin_familly == IPv4 주소, sin_addr == 소켓 사용 가능 네트워크 인터페이스, sin_port == 8080)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 수신되는 연결 대기, 에러 발생 시 종료
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // 연결 응답, 에러 발생 시 종료
    // buffer에서 상단에 작성했던 response 내용을 문자열 길이만큼 읽어와서 화면에 출력
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        write(new_socket, response, strlen(response));
        close(new_socket);
    }
    return 0;
}