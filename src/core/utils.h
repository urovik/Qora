#ifndef UTILS_H
#define UTILS_H



int set_nonblocking_fd(int sockfd);
void panic(const char* file, int line, const char* msg_err, ...);



#endif // UTILS_H
