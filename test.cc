// #include <iostream>
// #include <cstring>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <unistd.h>

// int main() {
//     // 创建 socket
//     int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (serverSocket == -1) {
//         perror("socket");
//         return 1;
//     }

//     // 绑定服务器地址和端口
//     struct sockaddr_in serverAddress;
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_port = htons(8080); // 使用端口号 8080
//     serverAddress.sin_addr.s_addr = INADDR_ANY;

//     if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
//         perror("bind");
//         close(serverSocket);
//         return 1;
//     }

//     // 开始监听连接请求
//     if (listen(serverSocket, 5) == -1) {
//         perror("listen");
//         close(serverSocket);
//         return 1;
//     }

//     std::cout << "Server listening on port 8080..." << std::endl;

//     while (true) {
//         // 接受客户端连接请求
//         struct sockaddr_in clientAddress;
//         socklen_t clientAddressLength = sizeof(clientAddress);
//         int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
//         if (clientSocket == -1) {
//             perror("accept");
//             continue;
//         }

//         std::cout << "Accepted a connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;

//         // 构造 HTTP 响应
//         std::string httpResponse = "HTTP/1.1 200 OK\r\n"
//                                    "Content-Type: text/html\r\n"
//                                    "\r\n"
//                                    "<html><body><h1>Hello, World!</h1></body></html>";

//         // 发送 HTTP 响应给客户端
//         ssize_t bytesSent = send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
//         if (bytesSent == -1) {
//             perror("send");
//         }

//         // 关闭客户端连接
//         close(clientSocket);
//     }

//     // 关闭服务器 socket
//     close(serverSocket);

//     return 0;
// }
