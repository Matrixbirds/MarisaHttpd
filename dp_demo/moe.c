#include <stdio.h>
#include <unistd.h>

#include <sys/utsname.h>
#include "kirisame.h"
#include "moe.h"

MarisaDPMain {

    char buf[32];

    struct utsname myuname;
    uname(&myuname);

    u_int8_t *arg = mdpGetUrlArg("mode");
    if ( arg != NULL )
        if ( strcmp(arg,"img") == 0 ) {
            mdpLEcho(__34689497_p0_jpg,__34689497_p0_jpg_len);
            return;
        }

    struct timeval tv_start;
    gettimeofday(&tv_start,NULL);

    mdpEcho("<!DOCTYPE html><html><head><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\"><title>MarisaHttpd v0.1</title></head>");
    mdpEcho("<body><h1>It works! XD</h1>");
    mdpEcho("This is MarisaHttpd v0.2 on ");
    mdpEcho(myuname.sysname);
    mdpEcho(" ");
    mdpEcho(myuname.release);
    mdpEcho(" ");
    mdpEcho(myuname.machine);
    mdpEcho(" <br><br>");
    mdpEcho("Marisa: Greetings from ");
    snprintf(buf,32,"%p",&con_info);
    mdpEcho(buf);
    mdpEcho(" ~<br>Your IP: ");
    mdpEcho(mdpGetClientIP);
    mdpEcho("<br>");
    mdpEcho("Your User-Agent: ");
    mdpEcho(mdpGetClientUserAgent);
    mdpEcho("<br><img src=\"moe?mode=img\"/><br>");
    mdpEcho("(<a href=\"http://www.pixiv.net/member_illust.php?mode=medium&amp;illust_id=34689497\">Image</a> by <a href=\"http://www.pixiv.net/member.php?id=292644\">えふぇ</a>)");
    mdpEcho("<hr>Powered by MarisaHttpd/0.2");

    struct timeval tv_end;
    gettimeofday(&tv_end,NULL);
    snprintf(buf,32,"%llu",(tv_end.tv_sec*100000+tv_end.tv_usec)-(tv_start.tv_sec*100000+tv_start.tv_usec));

    mdpEcho("<br>Processed in ");
    mdpEcho(buf);
    mdpEcho(" microseconds.<br>");

    mdpEcho("</body></html>");


}
