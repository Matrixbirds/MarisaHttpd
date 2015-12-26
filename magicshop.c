#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <inttypes.h>
#include <magic.h>
#include <dlfcn.h>

#include "magicshop.h"
#include "minihakkero.h"

u_int8_t *MarisaHttpd_GenerateDefaultErrorPage(u_int16_t errcode){
    u_int8_t msgbuf[64] = {0};
    u_int8_t pagebuf[1024] = {0};
    switch (errcode) {
    case 400:
        snprintf(msgbuf,64,"400 Bad Request");
        break;
    case 401:
        snprintf(msgbuf,64,"401 Unauthorized");
        break;
    case 402:
        snprintf(msgbuf,64,"402 Payment Required");
        break;
    case 403:
        snprintf(msgbuf,64,"403 Forbidden");
        break;
    case 404:
        snprintf(msgbuf,64,"404 Not Found");
        break;
    case 405:
        snprintf(msgbuf,64,"405 Method Not Allowed");
        break;
    case 406:
        snprintf(msgbuf,64,"406 Not Acceptable");
        break;
    case 407:
        snprintf(msgbuf,64,"407 Proxy Authentication Required");
        break;
    case 408:
        snprintf(msgbuf,64,"408 Request Timeout");
        break;
    case 409:
        snprintf(msgbuf,64,"409 Conflict");
        break;
    case 410:
        snprintf(msgbuf,64,"410 Gone");
        break;
    case 411:
        snprintf(msgbuf,64,"411 Length Required");
        break;
    case 412:
        snprintf(msgbuf,64,"412 Precondition Failed");
        break;
    case 413:
        snprintf(msgbuf,64,"413 Payload Too Large");
        break;
    case 414:
        snprintf(msgbuf,64,"414 URI Too Long");
        break;
    case 415:
        snprintf(msgbuf,64,"415 Unsupported Media Type");
        break;
    case 416:
        snprintf(msgbuf,64,"416 Range Not Satisfiable");
        break;
    case 417:
        snprintf(msgbuf,64,"417 Expectation Failed");
        break;
    case 418:
        snprintf(msgbuf,64,"418 I'm a teapot");
        break;
    case 419:
        snprintf(msgbuf,64,"419 Authentication Timeout");
        break;
    case 421:
        snprintf(msgbuf,64,"421 Misdirected Request");
        break;
    case 422:
        snprintf(msgbuf,64,"422 Unprocessable Entity");
        break;
    case 423:
        snprintf(msgbuf,64,"423 Locked");
        break;
    case 424:
        snprintf(msgbuf,64,"424 Failed Dependency");
        break;
    case 426:
        snprintf(msgbuf,64,"426 Upgrade Required");
        break;
    case 428:
        snprintf(msgbuf,64,"428 Precondition Required");
        break;
    case 429:
        snprintf(msgbuf,64,"429 Too Many Requests");
        break;
    case 431:
        snprintf(msgbuf,64,"431 Request Header Fields Too Large");
        break;
    case 451:
        snprintf(msgbuf,64,"451 Unavailable For (Il)legal Reasons");
        break;

    // You are not supposed to read these. However, maybe, this is the most proper way to record my feelings.
    case 460:
        snprintf(msgbuf,64,"460 Weeaboo Detected"); // My waifu!! Yaoi!! Yaoi!! Yaoi!! Yaoi!! Yaoi!!
        break;
    case 461:
        snprintf(msgbuf,64,"461 Resistance Agent Detected");
        /*
         * One Resistance Agent: Shonin? Lol! You Enlightened are just turbo chargers!
         * One Resistance Agent: Abaddon? Lol! You Enlightened are just turbo chargers!
         * One Resistance Agent: You are definitely a cheater. I have conclusive evidence.
         *
         * ** One Resistance Agent destroyed a 149-days portal **
         * ** One Resistance Agent destroyed a 149-days portal **
         * ** One Resistance Agent destroyed a 149-days portal **
         * ** One Resistance Agent destroyed a 149-days portal **
         * ** One Resistance Agent destroyed a 149-days portal **
         *
         * One Resistance Agent: We don't have Guardian portal searching system.
         *
         */
        break;
    case 462:
        snprintf(msgbuf,64,"462 You Are Not Welcomed By Gensokyo");
        abort(); // Intentionally. The Gensokyo? Overall, it is NOT a good place to put anyone's heart here.
        break;
    case 463:
        snprintf(msgbuf,64,"463 You Are Not Welcomed By Makai"); // Cheaters and traitors must leave my land.
        break;
    case 464:
        snprintf(msgbuf,64,"464 Patchouli or Nerd with the Same Characteristics Detected");
        /*
         * // This is a real story.
         *
         * // cirno *Cirno = (cirno *)me;
         *
         * // 2015-07-28
         * Patchouli: Cirno, I love you.
         * Cirno    : <3
         *
         * // Serveral days later
         * Patchouli: Cirno, I don't love you anymore.
         * Cirno    : Why?
         * Patchouli: When thinking rationally, you are full of bad habits.
         * Cirno    : What?
         * Patchouli: When thinking rationally, you are a scumbag.
         * Cirno    : Why?
         * Patchouli: Better not to say. Go away immediately.
         *
         * // More. Absurd but real.
         * Patchouli: I'm an architect. Your hardware&software won't work without me.
         * Patchouli: I'm very good at mathematics and physics. Even some university professors will have to ask me how to solve math problems.
         * Patchouli: I'm able to invent a new networking system to replace TCP/IP and OSI. But that's so boring, hahahaha.
         *
         */
        break;
    case 465:
        snprintf(msgbuf,64,"465 Resident of Scarlet Devil Mansion Detected");
        /*
         * Marisa: Their books...more books!!
         * Shinki: I'll support you, since they hurt me so much.
         */
        break;
    case 466:
        snprintf(msgbuf,64,"466 A Weeaboo&Traitor from Zhengzhou, Henan, China Detected"); // 2015.08
        break;
    case 467:
        snprintf(msgbuf,64,"467 The Future Is Unknown"); // 2015.12. Maybe live alone is the right choice for me.
        break;
    case 494:
        snprintf(msgbuf,64,"494 Request Header Too Large");
        break;
    case 500:
        snprintf(msgbuf,64,"500 Internal Server Error");
        break;
    case 501:
        snprintf(msgbuf,64,"501 Not Implemented");
        break;
    case 502:
        snprintf(msgbuf,64,"502 Bad Gateway");
        break;
    case 503:
        snprintf(msgbuf,64,"503 Service Unavailable");
        break;
    case 504:
        snprintf(msgbuf,64,"504 Gateway Timeout");
        break;
    case 505:
        snprintf(msgbuf,64,"505 HTTP Version Not Supported");
        break;
    case 506:
        snprintf(msgbuf,64,"506 Variant Also Negotiates");
        break;
    case 507:
        snprintf(msgbuf,64,"507 Insufficient Storage");
        break;
    case 508:
        snprintf(msgbuf,64,"508 Loop Detected");
        break;
    case 509:
        snprintf(msgbuf,64,"509 Bandwidth Limit Exceeded");
        break;
    case 510:
        snprintf(msgbuf,64,"510 Not Extended");
        break;
    case 511:
        snprintf(msgbuf,64,"511 Network Authentication Required");
        break;
    case 520:
        snprintf(msgbuf,64,"520 Unknown Error");
        break;
    default:
        snprintf(msgbuf,64,"%"PRIu16" Undefined Error Code",errcode);
        break;
    }

    snprintf(pagebuf,1024,"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\"><html><head><title>%s</title></head><body><h1>%s</h1><p>FIXME: Error description not implemented.</p><hr><address>MarisaHttpd/0.1</address></body></html>",msgbuf,msgbuf);

    return strdup(pagebuf);

}


u_int8_t *MarisaHttpd_DetermineFiletype(u_int8_t *filepath){ // TODO: NULL handling


    u_int8_t *magic_full = magic_file(magic_cookie, filepath);

    u_int8_t *ret = strdup(magic_full);

    return ret;
}
