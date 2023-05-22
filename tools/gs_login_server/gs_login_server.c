/*
    Copyright 2005-2010 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl-2.0.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "gs_login_proof.h"

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
    #define sleep   Sleep
    #define MYRAND  (u_int)GetTickCount()
    #define ONESEC  1000
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <pthread.h>
    #include <sys/times.h>

    #define MYRAND  (u_int)times(0)
    #define ONESEC  1
    #define stricmp strcasecmp
#endif

#ifdef WIN32
    #define quick_thread(NAME, ARG) DWORD WINAPI NAME(ARG)
    #define thread_id   DWORD
#else
    #define quick_thread(NAME, ARG) void *NAME(ARG)
    #define thread_id   pthread_t
#endif

thread_id quick_threadx(void *func, void *data) {
    thread_id       tid;
#ifdef WIN32
    if(!CreateThread(NULL, 0, func, data, 0, &tid)) return(0);
#else
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    //pthread_attr_setstacksize(&attr, 1<<18); //PTHREAD_STACK_MIN);
    if(pthread_create(&tid, &attr, func, data)) return(0);
#endif
    return(tid);
}



#define VER         "0.2.3b"
#define CPY(X,Y)    mystrcpy(X, Y, sizeof(X))
#define CMPCPY(x,y) if(!stricmp(par, x)) CPY(y, val);
//#define CMPNUM(x,y) if(!stricmp(par, x)) type = y;
#define LID         "1"



void generate_pids(u_char *fname);
int bind_job(u_short port);
quick_thread(client, int sd);
int send_login(int sd, ...);
void gamespy3dxor(u_char *data, int len);
int recv_parval(int sd, u_char *par, int parsz, u_char *val, int valsz, int *gsoff);
int mystrcpy(u_char *dst, u_char *src, int max);
u_char *create_rand_string(u_int seed, u_char *data, int len, u_char *table);
u_short crc16(u_short crc, unsigned char *data, int len);
int timeout(int sock, int secs);
void std_err(void);



typedef struct {
    int     pid;
    u_char  *user;
} mypids_t;

mypids_t    *mypids   = NULL;
int     sendlc1       = 1,
        verbose       = 0,
        gs_encoding   = 0,
        antifreeze    = 0;
static const u_char DEFAULT_NICK[] = "nick";
u_char  *default_nick = (u_char *)DEFAULT_NICK,
        *default_pass = "pass";

u_char *type2num[] = {
    "newuser",
    "login",
    "logout",
    "search",
    "others",
    "pmatch",
    "nicks",
    "auth",
    "authp",
    "getpd",
    "setpd",
    "getprofile",
    "check",
    "addbuddy",
    "status",
    "authadd",
    "valid",
    NULL
};



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer;
    int     sdl,
            sda,
            psz,
            i;
    u_short port;
    u_char  *pidfile    = NULL,
            *help_msg;

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    setbuf(stdout, NULL);

    fputs("\n"
        "GS login server emulator "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    if(argc < 2) {
        printf("\n"
            "Usage: %s [options] <port>\n"
            "\n"
            "Options:\n"
            "-p PASS   set the login password (%s)\n"
            "-n NICK   set the default nickname in case of problems (%s)\n"
            "-v        verbose output\n"
            "-f FILE   file in which is contained the list of usernames and pids to assign\n"
            "          the format is enough flexible so is possible to specify the username\n"
            "          and the pid on the same line (separated by a space) or on two lines\n"
            "          is also possible to delimit the username within \" or \' if desired\n"
            "-F        use this option if the clients freeze\n"
            "\n"
            "<port> can be 29900 (gpcm), 29901 (gpsp), 29920 (gamestats, XORed with\n"
            "\"GameSpy3D\") or any other known port\n"
            "\n", argv[0], default_pass, default_nick);
        exit(1);
    }

    argc--;
    for(i = 1; i < argc; i++) {
        if(((argv[i][0] != '-') && (argv[i][0] != '/')) || (strlen(argv[i]) != 2)) {
            printf("\nError: wrong argument (%s)\n", argv[i]);
            exit(1);
        }
        switch(argv[i][1]) {
            case 'v': verbose       = 1;            break;
            case 'q': verbose       = 0;            break;
            case 'n': default_nick  = argv[++i];    break;
            case 'p': default_pass  = argv[++i];    break;
            case 'f': pidfile       = argv[++i];    break;
            case 'F': antifreeze    = 1;            break;
            default: {
                printf("\nError: wrong command-line argument (%s)\n", argv[i]);
                exit(1);
                } break;
        }
    }
    port = atoi(argv[argc]);

    if(pidfile) generate_pids(pidfile);

    if(port == 29920) {
        printf("- xor encoding activated\n");
        gs_encoding = 1;
    } else if(port == 29901) {
        printf("- disable the initial sending of \\lc\\1\n");
        sendlc1 = 0;
    }

    printf(
        "- clients MUST use the password: %s\n"
        "- any nickname is valid and existent\n", default_pass);

    sdl = bind_job(port);
    switch(port) {
        case 29900: help_msg = " (gpcm.gamespy.com)";       break;
        case 29901: help_msg = " (gpsp.gamespy.com)";       break;
        case 29920: help_msg = " (*gamestats.gamespy.com)"; break;
        default:    help_msg = ""; break;
    }
    printf("- wait connections on port %u%s:\n", port, help_msg);

    for(;;) {
        psz = sizeof(struct sockaddr_in);
        sda = accept(sdl, (struct sockaddr *)&peer, &psz);
        if(sda < 0) {
            printf("- accept() failed, continue within one second\n");
            close(sdl);
            sleep(ONESEC);
            sdl = bind_job(port);
            continue;
        }

        printf("  %s:%u\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));

        if(!quick_threadx(client, (void *)sda)) close(sda);
    }

    close(sdl);
    return(0);
}



void generate_pids(u_char *fname) {
    FILE    *fd;
    int     pid,
            line,
            nextstep    = 0;
    u_char  buff[200],
            *user       = NULL,
            *p;

    printf("- load the pid file %s\n", fname);

    fd = fopen(fname, "rb");
    if(!fd) std_err();

    for(line = 0; fgets(buff, sizeof(buff), fd); line++);
    mypids = calloc(line + 1, sizeof(mypids_t));
    if(!mypids) std_err();

    fseek(fd, 0, SEEK_SET);
    line = 0;
    while(fgets(buff, sizeof(buff), fd)) {
        for(p = buff; *p && (*p != '\n') && (*p != '\r'); p++);
        *p = 0;
        for(p = buff - 1; p >= buff; p--) {
            if(*p <= ' ') *p = 0;
        }
        if(!buff[0]) continue;

        if(!nextstep) {
            pid  = 0;
            user = buff;
            while(*user && (*user <= ' ')) user++;
            if((*user == '"') || (*user == '\'')) {
                user++;
                for(p = user; *p && (*p != '"') && (*p != '\''); p++);
                *p = 0;
                for(p++; *p && (*p <= ' '); p++);
                pid = atoi(p);
            } else {
                p = strrchr(user, ' ');
                if(!p) p = strrchr(user, '\t');
                if(p) {
                    pid = atoi(p + 1);
                    for(p--; (p >= buff) && (*p <= ' '); p--);
                    p[1] = 0;
                }
            }
            user = strdup(user);    // needed here
            if(!p) {                // needed here
                nextstep = 1;
                continue;
            }
        } else {
            for(p = buff; *p && (*p <= ' '); p++);
            pid = atoi(p);
            nextstep = 0;
        }

        if(!user[0]) continue;
        printf("  pid %d for user \"%s\"\n", pid, user);
        mypids[line].pid  = pid;
        mypids[line].user = user;
        line++;
    }
    mypids[line].pid  = 0;  // useless because I used calloc
    mypids[line].user = NULL;
    fclose(fd);
}



int bind_job(u_short port) {
    struct  sockaddr_in peerx;
    int     sdl,
            on = 1;

    peerx.sin_addr.s_addr = INADDR_ANY;
    peerx.sin_port        = htons(port);
    peerx.sin_family      = AF_INET;

    sdl = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sdl < 0) std_err();
    if(setsockopt(sdl, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))
      < 0) std_err();
    if(bind(sdl, (struct sockaddr *)&peerx, sizeof(struct sockaddr_in))
      < 0) std_err();
    listen(sdl, SOMAXCONN);
    return(sdl);
}



void clean_nick(u_char *nick) {
    u_char  *p;

    p = strchr(nick, '@');
    if(p) *p = 0;
}



quick_thread(client, int sd) {
    int     i,
            ret,
            type,
            gsoff,
            mycrc            = 0;
    u_char  par[64]          = "",
            val[1024]        = "",
            mod[32]          = "",
            userid[32]       = "",
            profileid[32]    = "",
            xprofileid[32]   = "",
            sesskey[32]      = "",
            client_chall[64] = "",
            server_chall[11] = "",
            user[128]        = "",
            uniquenick[128]  = "",
            password[128]    = "",
            reason[1024]     = "",
            authtoken[1024]  = "",
            lt[25]           = "",
            id[32]           = "1",
            sig[33]          = "";

    sprintf(userid,    "%u", MYRAND);   // something pseudo-random, not important
    sprintf(profileid, "%u", MYRAND + 2);
    sprintf(sesskey,   "%u", MYRAND + 3);
    sprintf(mod,       "%u", MYRAND + 4);

    create_rand_string(MYRAND,
        sig, sizeof(sig),
        "0123456789abcdef");

    create_rand_string(MYRAND,
        server_chall, sizeof(server_chall),
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    create_rand_string(MYRAND,  // gamespy base64
        lt, sizeof(lt),
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ][");
    strcpy(lt + 22, "__");

    if(sendlc1) {
        if(send_login(sd,
            "lc",           "1",
            "challenge",    server_chall,
            "id",           id,
            NULL) < 0) goto give_up;
    }

    if(timeout(sd, 300)) goto give_up;
    for(;;) {
        if(verbose) printf("  %15s\n", "---");   // data block received

        gsoff = 0;
        type  = 0;
        for(;;) {
            ret = recv_parval(sd, par, sizeof(par), val, sizeof(val), &gsoff);
            if(ret < 0) goto give_up;
            if(ret == 1) break; // 1 = break now, 2 = break later

            if(verbose) printf("  %15s: %s\n", par, val);

            if(!type) {
                type = -1;
                for(i = 0; type2num[i]; i++) {
                    if(!stricmp(par, type2num[i])) {
                        type = i + 1;   // because type must be 0 if unavailable
                        break;
                    }
                }
                // "updatepro" doesn't need to be handled
            } else {
                     CMPCPY("userid",       userid)
                else CMPCPY("profileid",    profileid)
                else CMPCPY("email",        user)
                else CMPCPY("uniquenick",   uniquenick)
                else CMPCPY("user",         user)
                else CMPCPY("challenge",    client_chall)
                else CMPCPY("id",           id)
                else CMPCPY("reason",       reason)
                else CMPCPY("newprofileid", xprofileid)
                else CMPCPY("fromprofileid",xprofileid)
                else CMPCPY("authtoken",    authtoken)
            }

            if(ret) break;  // in case of a "\final\" with remaining data (gs_encoding)
        }

        if(verbose) printf("  %15s\n", "---");   // data block received

        if(default_nick != DEFAULT_NICK) {  // forced
            CPY(user, default_nick);
        }
        if(!user[0]) {
            if(uniquenick[0]) {
                CPY(user, uniquenick);
            } else {
                CPY(user, default_nick);
            }
        }
        if(!uniquenick[0]) CPY(uniquenick, user);
        clean_nick(uniquenick);
        if(!password[0])   CPY(password, default_pass);
        if(!mycrc && user[0]) {
            if(mypids) {
                for(i = 0; mypids[i].user; i++) {
                    if(!stricmp(user, mypids[i].user)) {
                        mycrc = mypids[i].pid;
                        break;
                    }
                }
            }   // in case there is not that user in mypids generate the pid
            if(!mycrc) mycrc = crc16(mycrc, user, strlen(user));
            sprintf(userid,    "%u", mycrc);
            sprintf(profileid, "%u", mycrc);
            sprintf(sesskey,   "%u", mycrc);
            sprintf(mod,       "%u", mycrc);
        }

        switch(type) {
            case 1: {   // newuser
                if(send_login(sd,
                    "nur",          "",
                    "userid",       userid,
                    "profileid",    profileid,
                    "id",           id,
                    NULL) < 0) goto give_up;
                break;
            }
            case 2: {   // login
                /*
                if(send_login(sd,
                    "blk",          "0",
                    "list",         "",
                    NULL) < 0) goto give_up;

                if(send_login(sd,
                    "bdy",          "1",
                    "list",         profileid,
                    NULL) < 0) goto give_up;
                */

                if(send_login(sd,
                    "lc",           "2",
                    "sesskey",      sesskey,
                    "proof",        gs_login_proof(password, authtoken[0] ? authtoken : user, client_chall, server_chall), // REQUIRED!
                    "userid",       userid,
                    "profileid",    profileid,
                    "uniquenick",   uniquenick,
                    "lt",           lt,
                    "id",           id,
                    NULL) < 0) goto give_up;

                /*
                if(send_login(sd,
                    "bm",           "100",
                    "f",            profileid,
                    "msg",          "|s|1|ss|Room Name|ls|gamename://gsp!gamename/?type=title|ip|0|p|0|qm|0",
                    NULL) < 0) goto give_up;
                */
                break;
            }
            case 3: {   // logout
                goto give_up;
                break;
            }
            case 4: {   // search
                if(send_login(sd,
                    /* the following is not necessary
                    "bsr",          profileid,
                    "nick",         uniquenick,
                    "firstname",    "",
                    "lastname",     "",
                    "email",        user,
                    "uniquenick",   uniquenick,
                    "namespace",    "0",*/
                    "bsrdone",      "",
                    NULL) < 0) goto give_up;
                goto give_up;   // the server disconnects the client
                break;
            }
            case 5: {   // others
                if(send_login(sd,
                    "odone",        "",
                    NULL) < 0) goto give_up;
                break;
            }
            case 6: {   // pmatch
                if(send_login(sd,
                    "psrdone",      "",
                    NULL) < 0) goto give_up;
                break;
            }
            case 7: {   // nicks
                if(send_login(sd,
                    "nr",           "1",  // user already exists
                    "nick",         uniquenick,
                    "uniquenick",   uniquenick,
                    //"nr",           "0",    // no user, needed only in registration (not needed because any nickname is valid)
                    "ndone",        "",
                    NULL) < 0) goto give_up;
                break;
            }
            case 8: {   // auth
                if(send_login(sd,
                    "lc",           "2",
                    "sesskey",      sesskey,
                    "proof",        "0",
                    "id",           id,
                    NULL) < 0) goto give_up;
                break;
            }
            case 9: {   // authp
                if(send_login(sd,
                    "pauthr",       profileid,
                    "lid",          LID,
                    NULL) < 0) goto give_up;
                break;
            }
            case 10: {  // getpd
                if(send_login(sd,
                    "getpdr",       "1",
                    "lid",          LID,
                    "pid",          profileid,
                    "mod",          mod,
                    "length",       "52",
                    "data",         "0000000000000000000000000000000000000000000000000000",
                    NULL) < 0) goto give_up;
                if(antifreeze) goto give_up;
                break;
            }
            case 11: {  // setpd
                if(send_login(sd,
                    "setpdr",       "1",
                    "lid",          LID,
                    "pid",          profileid,
                    "mod",          mod,
                    NULL) < 0) goto give_up;
                break;
            }
            case 12: {  // getprofile
                if(send_login(sd,
                    "pi",           "",
                    "profileid",    profileid,
                    "nick",         uniquenick,
                    "userid",       userid,
                    "email",        user,
                    "sig",          sig,
                    "uniquenick",   uniquenick,
                    "pid",          profileid,
                    "firstname",    "firstname",
                    "lastname",     "lastname",
                    "homepage",     "",
                    "zipcode",      "00000",
                    "countrycode",  "US",
                    "st",           "  ",
                    "birthday",     "0",
                    "sex",          "0",
                    "icquin",       "0",
                    "aim",          "",
                    "pic",          "0",
                    "pmask",        "64",
                    "occ",          "0",
                    "ind",          "0",
                    "inc",          "0",
                    "mar",          "0",
                    "chc",          "0",
                    "i1",           "0",
                    "o1",           "0",
                    "mp",           "4",    // "1073741831"
                    "lon",          "0.000000",
                    "lat",          "0.000000",
                    "loc",          "",
                    "conn",         "1",
                    "id",           id,
                    NULL) < 0) goto give_up;
                break;
            }
            case 13: {  // check
                if(send_login(sd,
                    "cur",          "0",
                    "pid",          profileid,
                    NULL) < 0) goto give_up;
                break;
            }
            case 14: {  // addbuddy
                if(send_login(sd,
                    "bm",           "2",
                    "f",            xprofileid,
                    "msg",          reason, //"Please let me add you to my PlayerSpy player list\r\n\r\n|signed|00000000000000000000000000000000",
                    NULL) < 0) goto give_up;
                break;
            }
            case 15: {  // status
                if(send_login(sd,
                    "bm",           "100",
                    "f",            profileid,
                    "msg",          "|s|1|ss|Room Name|ls|gamename://gsp!gamename/?type=title|ip|0|p|0|qm|0",
                    NULL) < 0) goto give_up;
                break;
            }
            case 16: {  // authadd
                if(send_login(sd,
                    "bm",           "1",
                    "f",            xprofileid,
                    "msg",          "I have authorized your request to add me to your list",
                    NULL) < 0) goto give_up;
                break;
            }
            case 17: {  // valid
                if(send_login(sd,
                    "vr",           "1",
                    "final",        "",
                    NULL) < 0) goto give_up;
                goto give_up;   // the server disconnects the client
                break;
            }
            default: {
                if(send_login(sd,
                    "pi",           "",
                    "pid",          profileid,
                    NULL) < 0) goto give_up;
                break;
            }
        }
    }

give_up:
    close(sd);
    printf("- disconnected\n");
    return(0);
}



int send_login(int sd, ...) {
    va_list ap;
    int     len;
    u_char  buff[1024],
            *p,
            *s;

    p = buff;
    va_start(ap, sd);
    while((s = va_arg(ap, u_char *))) {
        *p++ = '\\';
        p += mystrcpy(p, s, sizeof(buff) - (p - buff));
    }
    va_end(ap);
    if(verbose) printf("  %s\\final\\\n", buff);

    if(gs_encoding) gamespy3dxor(buff, p - buff);
    p += mystrcpy(p, "\\final\\", sizeof(buff) - (p - buff));

    len = p - buff;
    if(send(sd, buff, len, 0) != len) return(-1);
    return(0);
}



void gamespy3dxor(u_char *data, int len) {
    static const u_char gamespy[] = "GameSpy3D";
    u_char  *gs;

    gs = (u_char *)gamespy;
    while(len--) {
        *data++ ^= *gs++;
        if(!*gs) gs = (u_char *)gamespy;
    }
}



int recv_parval(int sd, u_char *par, int parsz, u_char *val, int valsz, int *gsoff) {
#define ISPARAMETER (!i)
    static const u_char gamespy[]     = "GameSpy3D",
                        fixed_final[] = "\\final\\";
    int     i,
            finaloff = 0;
    u_char  *p,
            *limit,
            *gs;

    gs     = (u_char *)gamespy + *gsoff;
    par[0] = 0;
    val[0] = 0;

    for(i = 0; i < 2; i++) {
        if(ISPARAMETER) {
            p = par;
            limit = par + parsz - 1;
        } else {
            p = val;
            limit = val + valsz - 1;
        }

        while(p < limit) {
            while(timeout(sd, 120) < 0) {   // useless keep-alive
                if(send_login(sd,
                    "ka",       "",
                    NULL) < 0) return(-1);
            }
            if(recv(sd, p, 1, 0) <= 0) return(-1);

            if(gs_encoding) {       // gs_encoding is boring to handle
                if(*p != fixed_final[finaloff]) finaloff = 0;
                if(*p == fixed_final[finaloff]) {
                    if(++finaloff >= (sizeof(fixed_final) - 1)) {
                        p++;        // it must be incremented
                        p -= (sizeof(fixed_final) - 1);
                        *p = 0;
                        if(p == par) return(1);
                        return(2);  // 2 because there are parts of val
                    }
                }
                *p ^= *gs++;
                if(!*gs) gs = (u_char *)gamespy;
            }

            if(*p == '\\') {
                if(p == par) continue;  // for the first '\', not 100% perfect
                break;
            }
            p++;
        }

        *p = 0;
        if(p >= limit) {
            printf("- the client sent a too big %s\n", ISPARAMETER ? "parameter" : "value");
            return(-1);
        }
        if(ISPARAMETER && !stricmp(par, "final")) { // "\final\"
            par[0] = 0; // useless
            return(1);
        }
    }

    *gsoff = gs - (u_char *)gamespy;
    return(0);
}



int mystrcpy(u_char *dst, u_char *src, int max) {
    u_char  *p = dst;

    while(*src && (--max > 0)) {
        *p++ = *src++;
    }
    *p = 0;
    return(p - dst);
}



u_char *create_rand_string(u_int seed, u_char *data, int len, u_char *table) {
    int     tablelen = strlen(table);
    u_char  *p = data;

    while(--len > 0) {
        seed = (seed * 0x343FD) + 0x269EC3;
        seed >>= 1; // blah, sometimes useful
        *p++ = table[seed % tablelen];
    }
    *p = 0;
    return(data);
}



u_short crc16(u_short crc, unsigned char *data, int len) {
    static const u_short crc_lut[256] = {
        0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
        0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
        0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
        0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
        0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
        0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
        0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
        0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
        0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
        0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
        0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
        0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
        0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
        0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
        0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
        0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
        0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
        0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
        0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
        0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
        0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
        0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
        0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
        0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
        0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
        0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
        0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
        0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
        0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
        0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
        0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
        0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
    };

    while(len--) {
        crc = crc_lut[(*data ^ crc) & 0xff] ^ (crc >> 8);
        data++;
    }
    return(crc);
}



int timeout(int sock, int secs) {
    struct  timeval tout;
    fd_set  fd_read;
    int     err;

    tout.tv_sec  = secs;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    err = select(sock + 1, &fd_read, NULL, NULL, &tout);
    if(err < 0) return(-1); //std_err();
    if(!err) return(-1);
    return(0);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif



