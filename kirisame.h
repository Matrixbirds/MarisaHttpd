#ifndef MARISA_KIRISAME_H
#define MARISA_KIRISAME_H

#include <microhttpd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MarisaHttpdVersion              0100

#define Kirisame_DataPool_Idle          100
#define Kirisame_DataPool_InProgress    101
#define Kirisame_DataPool_Finished      102

#define __mdpInternalSafeChk            if ( con_info == NULL || data == NULL ) return

#define MarisaDPMain int MarisaDPAPIVersion(){\
return MarisaHttpdVersion;\
}\
void DPMain(struct ConnectionInfo *con_info)



// NOTE: DO NOT free() the returned value. They will be free'd after the connection is closed.
#define mdpGetUrlArg(arg)                      __mdpGetUrlArg(con_info,arg)
#define mdpGetUrlArgLen(arg)                   __mdpGetUrlArgLen(con_info,arg)
#define mdpGetPostData(arg)                    __mdpGetPostData(con_info,arg)
#define mdpGetPostDataLen(arg)                 __mdpGetPostDataLen(con_info,arg)
#define mdpGetClientHTTPHeaderValue(arg)       (u_int8_t *)MHD_lookup_connection_value(con_info->Conn,MHD_HEADER_KIND,arg)
#define mdpGetClientUserAgent                  mdpGetClientHTTPHeaderValue("User-Agent")
#define mdpGetClientConnectionMode             mdpGetClientHTTPHeaderValue("Connection")
#define mdpGetClientHostName                   mdpGetClientHTTPHeaderValue("Host")
#define mdpGetClientIP                         inet_ntoa(con_info->ClientInfo.Addr->sin_addr)


#define mdpEcho(arg)                           __mdpEcho(con_info,arg)
#define mdpLEcho(arg,len)                      __mdpLEcho(con_info,arg,len)
#define mdpSetRespCode(arg)                    __mdpSetRespCode(con_info,arg)



// NOTE: Free me after use.
#define mdpGetMDPVersion                       mdp_itoa(MarisaHttpdVersion)


struct Args {
        size_t nr_Args;
        u_int8_t **ArgName;
        u_int8_t **ArgValue;
        size_t *ArgLen;
};

struct DataPool {
        int Status;
        size_t DataLen;
        u_int8_t *Data;
};


struct ClientInfo {
        struct sockaddr_in *Addr;
        //u_int8_t *UserAgent;

};

struct DPContainerThreadInfo {
        struct ConnectionInfo *con_info;
        void (*ThisDPMain)(struct ConnectionInfo *con_info);
};

struct ConnectionInfo {
    int HTTPMethod;
    struct MHD_Connection *Conn;
    struct MHD_PostProcessor *PostDataProcessor;
    struct ClientInfo ClientInfo;
    int HasUrlArgs;
    struct Args *UrlArgs; // Arguments in URL
    struct Args *PostData;
    struct Args *RespHdrs;
    int RespCode; // 200, 404, etc
    void *DLHandle; // Handle to opened DL library

    struct DataPool DataPool;

};

u_int8_t *mdp_itoa(int i);
u_int8_t *__mdpGetUrlArg(struct ConnectionInfo *con_info, u_int8_t *key);
size_t __mdpGetUrlArgLen(struct ConnectionInfo *con_info, u_int8_t *key);
u_int8_t *__mdpGetPostData(struct ConnectionInfo *con_info, u_int8_t *key);
size_t __mdpGetPostDataLen(struct ConnectionInfo *con_info, u_int8_t *key);
void __mdpEcho(struct ConnectionInfo *con_info, u_int8_t *data);
void __mdpLEcho(struct ConnectionInfo *con_info, u_int8_t *data, size_t len);
void __mdpSetRespCode(struct ConnectionInfo *con_info, u_int16_t code);


#endif
