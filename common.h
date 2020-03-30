#ifndef H_COMMON
#define H_COMMON

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define VERSION "1.0.0a"
#define OK  0
#define ERR -1

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


extern int errno;


/* Used to create string literal from integer during preprocess. */
#define _STR(X) #X
#define STR(X) _STR(X)


#define LOG(file, ...) do {	\
	fprintf(file, __VA_ARGS__); \
	fprintf(file, "\n" ); \
    if ((file == stderr) && errno) { \
        fprintf(file, "Additional info: %s\n", strerror(errno)); \
    } \
	fflush(file); \
} while(0)
 

#define printfln( ... ) LOG(stdout, __VA_ARGS__)
#define perrorf( ... ) LOG(stderr, __VA_ARGS__)
#define printsocket(m, a) \
    printf("%s%s:%d\n", m, inet_ntoa(a.sin_addr), ntohs(a.sin_port))


#define DEFAULT_BAUDRATE    115200


struct Settings {
	char *device;
    char *serialdevice;
    unsigned int baudrate;
};


volatile struct Settings settings;

#endif
