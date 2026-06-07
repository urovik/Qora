#ifndef UTILS_H
#define UTILS_H



int set_nonblocking_fd(int sockfd);
void panic(const char* file, int line, const char* msg_err, ...);

unsigned int MurmurHash2 (char* key, unsigned int len);

#endif // UTILS_H
