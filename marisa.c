#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include <fcntl.h>
#include <dlfcn.h>
#include <pthread.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <microhttpd.h>
#include <magic.h>

#include "magicshop.h"
#include "minihakkero.h"
#include "kirisame.h"

magic_t magic_cookie;

int socket_mode_xd;
int dl_dont_close = 0;
static u_int16_t nr_Clients = 0;
static char *rootdir = NULL;
size_t rootdir_len = 0;


static int SendFile(struct MHD_Connection *connection, u_int8_t *filepath){
    int ret;
    struct MHD_Response *response;
    u_int8_t *content_type;

    int filefd = open(filepath,0);

    if ( filefd < 0 ) return MHD_NO;

    response = MHD_create_response_from_fd(utils_GetFileSize(filepath),filefd);

    if (!response) return MHD_NO; // If failed to create a response

    content_type = MarisaHttpd_DetermineFiletype(filepath);

    if ( MHD_add_response_header(response,"Content-Type",content_type) == MHD_NO ) return MHD_NO;

    free(content_type);
    free(filepath);

    ret = MHD_queue_response(connection, 200, response);
    MHD_destroy_response(response);

    return ret;
}

static int SendErrorPage(struct MHD_Connection *connection, u_int16_t errcode){
    int ret;
    struct MHD_Response *response;

    u_int8_t *errpage = MarisaHttpd_GenerateDefaultErrorPage(errcode);

    response = MHD_create_response_from_buffer(strlen(errpage), (void *)errpage, MHD_RESPMEM_MUST_FREE);
    if (!response) return MHD_NO; // If failed to create a response

    ret = MHD_queue_response(connection, errcode, response);

    MHD_destroy_response(response);

    return ret;
}





static int AppendUrlArgs(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
    struct Args *urlargs = cls;

    urlargs->nr_Args++;

    marisa_say(2,"AppendUrlArgs: cls=%p, Num=%d, Key=%s, Value=%s",cls,urlargs->nr_Args,key,value);
    //marisa_say(2,"AppendUrlArgs: Num=%d, Key=%s, Value=%s",urlargs->nr_Args,key,value);

    urlargs->ArgName = realloc(urlargs->ArgName,urlargs->nr_Args*sizeof(char *));
    urlargs->ArgValue = realloc(urlargs->ArgValue,urlargs->nr_Args*sizeof(char *));

    urlargs->ArgName[urlargs->nr_Args-1] = strdup(key);
    urlargs->ArgValue[urlargs->nr_Args-1] = strdup(value);

    return MHD_YES;

}

static void ThinkLife(){

}

static void FreeUrlArgs(struct Args *urlargs){
    size_t j;


    for (j=0;j<urlargs->nr_Args;j++){
        free(urlargs->ArgName[j]);
        free(urlargs->ArgValue[j]);
    }

    if ( urlargs->nr_Args ) {
        free(urlargs->ArgName);
        free(urlargs->ArgValue);
    }


    free(urlargs);
}

static void FreePostData(struct Args *pd){
    size_t j;


    for (j=0;j<pd->nr_Args;j++){
        free(pd->ArgName[j]);
        free(pd->ArgValue[j]);
    }

    if ( pd->nr_Args ) {
        free(pd->ArgName);
        free(pd->ArgValue);
    }

    free(pd->ArgLen);

    free(pd);
}

static void ResourceCleaner (void *cls, struct MHD_Connection *connection,
                             void **con_cls, enum MHD_RequestTerminationCode toe)
{
    struct ConnectionInfo *con_info = *con_cls;

    if (NULL == con_info)
        return;

    if ( con_info->PostData != NULL )
        FreePostData(con_info->PostData);


    free (con_info);
    *con_cls = NULL;
}

static u_int8_t *UrlToRealpath(u_int8_t *url){ // Generate the real path
    if ( url == NULL ) {
        marisa_say(2,"[UrlToRealpath] WARNING: Input URL string is NULL.");
        return NULL;
    }

    size_t total_len = rootdir_len+strlen(url)+2;

    u_int8_t *ret = malloc(total_len);

    ret[0] = 0;

    if ( ret == NULL ) {
        marisa_say(2,"[UrlToRealpath] Ahh!! My broom becomes NULL!!!");
    }

    snprintf(ret,total_len,"%s/%s",rootdir,url);

    return ret;
}

static int PostDataProcessor (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
                              const char *filename, const char *content_type,
                              const char *transfer_encoding, const char *data, uint64_t off,
                              size_t size)
{

    struct ConnectionInfo *con_info = coninfo_cls;

    size_t pos = 0;
    size_t oldatapos = 0;

    if ( size < 1 ) return MHD_NO;



    marisa_say(3,"xxx %zu",con_info->PostData->nr_Args);

    for ( pos=0;pos<con_info->PostData->nr_Args;pos++) {
        if ( 0 == strcmp(key,con_info->PostData->ArgName[pos]) ) {

            oldatapos = con_info->PostData->ArgLen[pos];

            con_info->PostData->ArgLen[pos] += size;

            con_info->PostData->ArgValue[pos] = realloc(con_info->PostData->ArgValue[pos],con_info->PostData->ArgLen[pos]);

            marisa_say(2,"[PostDataProcessor] [coninfo %p] [Append] Key:%s, Start:%p, AppendPos:%p, Oldsize:%zu, NewSize:%zu",coninfo_cls,key,con_info->PostData->ArgValue[pos],con_info->PostData->ArgValue[pos]+oldatapos,oldatapos,con_info->PostData->ArgLen[pos]);

            memcpy(con_info->PostData->ArgValue[pos]+oldatapos,data,size);

            return MHD_YES;

            break;
        }
    }

    pos = con_info->PostData->nr_Args;

    con_info->PostData->nr_Args++;

    con_info->PostData->ArgName = realloc(con_info->PostData->ArgName,con_info->PostData->nr_Args*sizeof(u_int8_t *));
    con_info->PostData->ArgValue = realloc(con_info->PostData->ArgValue,con_info->PostData->nr_Args*sizeof(u_int8_t *));
    con_info->PostData->ArgLen = realloc(con_info->PostData->ArgLen,con_info->PostData->nr_Args*sizeof(size_t));



    con_info->PostData->ArgName[pos] = strdup(key);
    con_info->PostData->ArgValue[pos] = malloc(size);
    con_info->PostData->ArgLen[pos] = size;

    marisa_say(2,"[PostDataProcessor] [coninfo %p] [Add] Key:%s, Data:%p Size:%zu",coninfo_cls,key,con_info->PostData->ArgValue[pos],size);

    memcpy(con_info->PostData->ArgValue[pos],data,size);


    return MHD_YES;

}

static int ConnectionHandler(void *cls, struct MHD_Connection *connection,
                             const char *url, const char *method,
                             const char *version, const char *upload_data,
                             size_t *upload_data_size, void **con_cls)
{
    if (NULL == *con_cls) // If con_cls is NULL, then this is a newly-created connection
    {
        struct ConnectionInfo *con_info;




        con_info = malloc (sizeof (struct ConnectionInfo)); // malloc it
        if (NULL == con_info) return MHD_NO; // If my broom becomes NULL, give up

        con_info->RespCode = 200;
        con_info->HasUrlArgs = 0;
        con_info->DataPool.DataLen = 0;
        con_info->DataPool.Data = NULL;




        union MHD_ConnectionInfo *ThisConnInfo;

        ThisConnInfo = MHD_get_connection_info(connection,MHD_CONNECTION_INFO_CLIENT_ADDRESS);

        con_info->ClientInfo.Addr = (struct sockaddr_in *)ThisConnInfo->client_addr;


        con_info->Conn = connection;





        if (0 == strcmp (method, "POST"))
        {


            con_info->PostData = malloc(sizeof(struct Args));
            con_info->PostData->ArgLen = NULL;
            con_info->PostData->ArgName = NULL;
            con_info->PostData->ArgValue = NULL;
            con_info->PostData->nr_Args = 0;

            con_info->PostDataProcessor = MHD_create_post_processor (connection, 1024,
                                                                     PostDataProcessor, (void *) con_info);

            if (NULL == con_info->PostDataProcessor)
            {
                free (con_info);
                return MHD_NO;
            }

        } else {
            con_info->PostData = NULL;
        }

        marisa_say(1,"[ConnectionHandler] [ConInfo %p] Incoming new connection from %s:%"PRIu16,con_info,inet_ntoa(con_info->ClientInfo.Addr->sin_addr),ntohs(con_info->ClientInfo.Addr->sin_port));



        *con_cls = (void *) con_info; // Store info

        return MHD_YES; // Accept it first, and I'll process it the next time
    }

    // If con_cls is NOT NULL, then this is a previously-created connection



    struct ConnectionInfo *con_info = *con_cls;

    if ( strcmp(method,"POST") == 0 ) {
        if (*upload_data_size > 0)
        {


            marisa_say(2,"[ConnectionHandler] [ConInfo %p] Start PostProcessing.",con_info);
            MHD_post_process(con_info->PostDataProcessor, upload_data,*upload_data_size);

            *upload_data_size = 0;

            return MHD_YES;

        } else {

            marisa_say(2,"[ConnectionHandler] [ConInfo %p] Stop PostProcessing.",con_info);
            MHD_destroy_post_processor(con_info->PostDataProcessor);
        }

    }


    //if ( upload_data != NULL ) free(upload_data);



    // First I'll check content existence, no matter the method is GET or POST or whatever

    u_int8_t *realpath = UrlToRealpath(url);

    u_int8_t *filetype = NULL;

    u_int16_t errcode = 500;

    if ( realpath == NULL ) return MHD_NO;


    if ( utils_file_exist(realpath) ) {
        filetype = MarisaHttpd_DetermineFiletype(realpath);

        if ( strcmp(filetype,"application/x-sharedlib") == 0 ) {
            free(filetype);
            return SendErrorPage(connection,403);
        }

        marisa_say(0,"[ConnectionHandler] [ConInfo %p] User requested %s, Type: StaticFile, RealPath is %s, Type is %s",con_info,url,realpath,filetype);

        free(filetype);
        return SendFile(connection,realpath);

    } else if ( utils_file_exist(realpath = utils_AppendString(realpath,".so")) ) {

        filetype = MarisaHttpd_DetermineFiletype(realpath);

        marisa_say(0,"[ConnectionHandler] [ConInfo %p] User requested %s, Type: DP, RealPath is %s, Type is %s",con_info,url,realpath,filetype);

        if ( strcmp(filetype,"application/x-sharedlib") == 0 ) {

            con_info->DLHandle = dlopen(realpath,RTLD_LAZY);
            if ( !con_info->DLHandle ) { // dlopen failed
                marisa_say(1,"Error occured while opening magic book: %s",realpath);
                goto badending;
            }

            int (*VersionChk)();
            VersionChk = dlsym(con_info->DLHandle,"MarisaDPAPIVersion");

            if ( VersionChk == NULL ) { // DL symbol load failed
                dlclose(con_info->DLHandle);
                marisa_say(1,"Error occured while reading magic book: %s",realpath);
                goto badending;
            }

            if ( VersionChk() != MarisaHttpdVersion ) { // API version check failed
                dlclose(con_info->DLHandle);
                marisa_say(1,"This magic book is either too old or too new: %s",realpath);
                goto badending;
            }

            // Now everything is Okay, time to rock


            // Get arguments in URL, e.g. foo.html?a=1&b=2

            con_info->UrlArgs = malloc(sizeof(struct Args));

            con_info->UrlArgs->nr_Args = 0;
            con_info->UrlArgs->ArgName = NULL;
            con_info->UrlArgs->ArgValue = NULL;

            con_info->HasUrlArgs = 1;

            MHD_get_connection_values(connection,MHD_GET_ARGUMENT_KIND,&AppendUrlArgs,(void *)con_info->UrlArgs);

            // Launch DL

            void (*ThisDPMain)(struct ConnectionInfo *con_info);

            ThisDPMain = dlsym(con_info->DLHandle,"DPMain");

            if ( ThisDPMain == NULL ) {
                dlclose(con_info->DLHandle);
                marisa_say(1,"[ConnectionHandler] [ConInfo %p] Error occured while using magic: %s",con_info,realpath);
                goto badending;
            }

            marisa_say(3,"[ConnectionHandler] [ConInfo %p] Running DP %p.",con_info,ThisDPMain);

            ThisDPMain(con_info);

            if ( !dl_dont_close ) dlclose(con_info->DLHandle);
            // Bring up output channel first

            int ResponseQueueResult;
            struct MHD_Response *response;

            marisa_say(3,"[ConnectionHandler] [ConInfo %p] DP returned: Len:%zu Data:%p",con_info,con_info->DataPool.DataLen,con_info->DataPool.Data);

            response = MHD_create_response_from_buffer(con_info->DataPool.DataLen,con_info->DataPool.Data,MHD_RESPMEM_MUST_FREE);

            if (!response) return MHD_NO; // If failed to create a response


            ResponseQueueResult = MHD_queue_response(connection, con_info->RespCode, response);



            MHD_destroy_response(response);




            if ( con_info->HasUrlArgs ) FreeUrlArgs(con_info->UrlArgs);
            free(filetype);
            free(realpath);

            marisa_say(0,"[ConnectionHandler] [ConInfo %p] Connection cycle finished.",con_info);
            return ResponseQueueResult;


        } else {
            errcode = 500;
            goto badending;
        }


    } else {
        errcode = 404;
        goto badending;
    }


badending:
    if ( con_info->HasUrlArgs ) FreeUrlArgs(con_info->UrlArgs);
    if ( filetype != NULL ) free(filetype);
    free(realpath);
    return SendErrorPage(connection,errcode);


}


static int ServiceMode(u_int16_t port, u_int8_t *ipaddr){
    struct MHD_Daemon *daemon;

    struct sockaddr_in ThisSockAddr;
    ThisSockAddr.sin_family = AF_INET;
    ThisSockAddr.sin_port = htons(port);
    inet_aton(ipaddr,&ThisSockAddr.sin_addr);


    daemon = MHD_start_daemon(MHD_USE_EPOLL_INTERNALLY_LINUX_ONLY, port, NULL, NULL,
                              &ConnectionHandler, NULL,
                              MHD_OPTION_NOTIFY_COMPLETED, ResourceCleaner,
                              NULL, MHD_OPTION_SOCK_ADDR, (struct sockaddr *)&ThisSockAddr,MHD_OPTION_END);
    if (NULL == daemon) {
        marisa_say(2,"Uh...Something went wrong...");
        exit(1);
    }
    while(1) sleep(65535);


    return 0;
}


static void ShowLicense(){
    fprintf(stderr,"%s\n",VersionInfo);
}

static void ShowHelp(){
    fprintf(stderr,"This is MarisaHttp v0.2.\n\n"
            "Usage: marisa -m <epoll|select> -d <rootdir> -b <ip> -p <port> [-n] [-v]\n\n"
            "    -m    Socket handling mode. epoll is only supported on Linux.\n"
            "    -d    Root directory.\n"
            "    -b    IP address to bind to.\n"
            "    -p    Port to bind to.\n"
            "    -n    Don't dlclose() after invocations. Improves speed but changes to DP won't shown unless the server is restarted.\n"
            "    -v    Show version and license info.\n"

            );

}

int main(int argc, char *argv[])
{

    VersionInfo[205] = '0';
    VersionInfo[235] = '2';


    pthread_mutex_init(&perrormutex,NULL);


    magic_cookie = magic_open(MAGIC_MIME_TYPE);
    magic_load(magic_cookie, NULL);

    char *socketmode = NULL;
    char *bindaddr = NULL;
    char *portnum = NULL;

    int c;

    opterr = 0;
    while ((c = getopt (argc, argv, "m:b:p:d:vn")) != -1)
        switch (c)
        {
        case 'm':
            socketmode = optarg;
            break;
        case 'b':
            bindaddr = optarg;
            break;
        case 'p':
            portnum = optarg;
            break;
        case 'd':
            rootdir = optarg;
            break;
        case 'v':
            ShowLicense();
            exit(0);
            break;
        case 'n':
            dl_dont_close = 1;
            break;
        case '?':
            if ( optopt == 'm' ) {
                goto nosocketmode;
            } else if ( optopt == 'b' ) {
                goto nobindaddr;
            } else if ( optopt == 'd' ) {
                goto nodir;
            } else if ( optopt == 'p' ) {
                goto noportnum;
            } else if ( isprint (optopt) ) {
                marisa_say(1,"-%c? Hmm, I don't know that.",optopt);
                goto help;
            } else {
                marisa_say(1,"Unknown option character `\\x%x'.",optopt);
            }
            return 1;
        default:
            goto badend;
        }

    if (socketmode == NULL) {
        goto nosocketmode;
    }
    if (bindaddr == NULL) {
        goto nobindaddr;
    }
    if (portnum == NULL) {
        goto noportnum;
    }
    if (rootdir == NULL) {
        goto nodir;
    }

    goto allok;

nosocketmode:
    marisa_say(1,"Please specify socket handling mode.");
    goto help;

nodir:
    marisa_say(1,"Please specify root directory.");
    goto help;


nobindaddr:
    marisa_say(1,"Please specify the address to bind.");
    goto help;

noportnum:
    marisa_say(1,"Please specify the port to listen on.");
    goto help;


allok:

    if ( 0 == strcmp(socketmode,"epoll") ) {
        socket_mode_xd = MHD_USE_EPOLL_LINUX_ONLY;
    } else if ( 0 == strcmp(socketmode,"select") ) {
        socket_mode_xd = MHD_USE_SELECT_INTERNALLY;
    } else {
        marisa_say(1,"Socket mode must be either `epoll' or `select'.");
        goto badend;
    }

    marisa_say(0,"Greetings <3");
    marisa_say(0,"Socket mode: %s, Listening on %s:%s",socketmode,bindaddr,portnum);


    rootdir_len = strlen(rootdir);
    marisa_say(0,"Root directory: %s",rootdir);

    ServiceMode(atoi(portnum),bindaddr);
    goto goodend;

help:

    ShowHelp();
    goto badend;

goodend:
    return 0;

badend:
    return 1;
}


