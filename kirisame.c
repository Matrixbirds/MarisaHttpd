#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include <pthread.h>

#include "kirisame.h"
#include "magicshop.h"
#include "minihakkero.h"


u_int8_t *mdp_itoa(int i){
    u_int8_t buf[16];
    snprintf(buf,16,"%d",i);
    u_int8_t *fbuf = strdup(buf);

    return fbuf;
}

u_int8_t *__mdpGetUrlArg(struct ConnectionInfo *con_info, u_int8_t *key){

    size_t j;

    if ( con_info->HasUrlArgs ) {

        for(j=0;j<con_info->UrlArgs->nr_Args;j++) {
            if ( 0 == strcmp(con_info->UrlArgs->ArgName[j],key) ) return con_info->UrlArgs->ArgValue[j];
        }

    }

    return NULL;
}

size_t __mdpGetUrlArgLen(struct ConnectionInfo *con_info, u_int8_t *key){

    size_t j;

    if ( con_info->HasUrlArgs ) {
        for(j=0;j<con_info->UrlArgs->nr_Args;j++) {
            if ( 0 == strcmp(con_info->UrlArgs->ArgName[j],key) ) return con_info->UrlArgs->ArgLen[j];
        }

        return 0;
    }
}
u_int8_t *__mdpGetPostData(struct ConnectionInfo *con_info, u_int8_t *key){

    if ( con_info == NULL || con_info->PostData == NULL || key == NULL ) return NULL;

    size_t j;

    for(j=0;j<con_info->PostData->nr_Args;j++) {
        if ( 0 == strcmp(con_info->PostData->ArgName[j],key) ) return con_info->PostData->ArgValue[j];
    }

    return NULL;
}

size_t __mdpGetPostDataLen(struct ConnectionInfo *con_info, u_int8_t *key){

    if ( con_info == NULL || con_info->PostData == NULL || key == NULL ) return 0;

    size_t j;

    for(j=0;j<con_info->PostData->nr_Args;j++) {
        if ( 0 == strcmp(con_info->PostData->ArgName[j],key) ) return con_info->PostData->ArgLen[j];
    }

    return 0;
}

void __mdpEcho(struct ConnectionInfo *con_info, u_int8_t *data){

    __mdpInternalSafeChk;

    size_t len = strlen(data);

    size_t origpos = con_info->DataPool.DataLen;

    //marisa_say(1,"mdpEcho: Origpos %zu",origpos);
    con_info->DataPool.DataLen += len;

    con_info->DataPool.Data = realloc(con_info->DataPool.Data,con_info->DataPool.DataLen);

    memcpy(con_info->DataPool.Data+origpos,data,len);

    //marisa_say(0,"mdpEcho: Wrote %zd bytes",n);


}

void __mdpLEcho(struct ConnectionInfo *con_info, u_int8_t *data, size_t len){

    __mdpInternalSafeChk;

    size_t origpos = con_info->DataPool.DataLen;

    con_info->DataPool.DataLen += len;

    con_info->DataPool.Data = realloc(con_info->DataPool.Data,con_info->DataPool.DataLen);

    memcpy(con_info->DataPool.Data+origpos,data,len);

}

void __mdpSetRespCode(struct ConnectionInfo *con_info, u_int16_t code){
    con_info->RespCode = code;
}


