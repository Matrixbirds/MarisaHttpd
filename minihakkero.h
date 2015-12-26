#ifndef MARISA_HAKKERO_H
#define MARISA_HAKKERO_H

#include <inttypes.h>

#define marisa_say(msg_type,fmt,...) pthread_mutex_lock(&perrormutex);utils_func_ptime();utils_func_pcol(msg_type);fprintf(stderr,"Marisa: "fmt "\e[0m\n",##__VA_ARGS__);pthread_mutex_unlock(&perrormutex)
#define marisa_say_nn(msg_type,fmt,...) pthread_mutex_lock(&perrormutex);utils_func_ptime();utils_func_pcol(msg_type);fprintf(stderr,"Marisa: "fmt "\e[0m",##__VA_ARGS__);pthread_mutex_unlock(&perrormutex)

unsigned char VersionInfo[11656];

pthread_mutex_t perrormutex ;
//unsigned short int perror_lock;

void MarisaHttpd_ShowVersion();
void *utils_func_pcol(unsigned short int msg_type);
void *utils_func_ptime(void);
off_t utils_GetFileSize(u_int8_t *path);
int utils_file_exist (char *filename);
u_int8_t *utils_AppendString(u_int8_t *s0, u_int8_t *s1);
#endif
