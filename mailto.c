/*
** mailto2 - WWW form to mail gateway
**
** Copyright (c) 1994-1999 by Andreas Ley <ley@rz.uni-karlsruhe.de>
** Copyright (c) 1998,2004 by Martin Schulze <joey@infodrom.org>
** Copyright (c) 2017 by Differentiated Analytics Inc.
**                       <info@differentiatedanalytics.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "mailto_english.h"

#include <netdb.h>
#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File locations */

#define ADDRESSES "/etc/mailto.conf"
#define MAILCMD "/usr/lib/sendmail -bs >/dev/null"

/* General definitions */

#define POSTMASTER "postmaster"
#define WEBMASTER "webmaster"
#define SENDER "www"

/* Global variables */

#define BUFSIZE 8192
#define MAXFIELDS 1024

char header[] = "mailto2 2.0.0\n"
"(c) 1994-1999 by Andreas Ley <Andreas.Ley@rz.uni-karlsruhe.de>\n"
"(c) 1998,2004 by Martin Schulze <joey@infodrom.org>\n"
"(c) 2017 by Differentiated Analytics Inc. <info@differentiatedanalytics.ca>\n";

char fqdn[256];
char mailname[256];
char *htag[MAXFIELDS], *hval[MAXFIELDS];
size_t max;
int debug = 0;


/* print header of an HTML response */
void show_header(char *subtitle, char *description)
{
    if (debug) fprintf(stderr, "show_header(\"%s\",\"%s\")\n", subtitle, description);

    printf("Content-type: text/html\n\n");
    printf("<!DOCTYPE html>\n");
    printf("<!--\n%s-->\n", header);
    printf("<html>\n");
    printf("  <head>\n");
    printf("    <meta charset=\"utf-8\" />\n");
    printf("    <title>%s%s%s</title>\n", TITLE, subtitle ? " - " : "" , subtitle ? subtitle : "");
    printf("  </head>\n");
    printf("  <body>\n");
    printf("    <h1>%s</h1>\n", subtitle ? subtitle : TITLE);
    if (description) {
        printf("<p>");
        printf(description, WEBMASTER, mailname);
        printf("</p>\n");
    }
}


/* print footer (trailer) of an HTML response */
void show_trailer()
{
    if (debug) fprintf(stderr, "show_trailer()\n");

    printf("  </body>\n");
    printf("</html>\n");
}


/* print an error page (wrong user input) */
void show_error(char *error)
{
    if (debug) fprintf(stderr, "show_error(\"%s\")\n", error);

    show_header(ERROR, error);
    show_trailer();
    exit(0);
}


/* print an error page (internal malfunction) */
void show_fatal(char *error)
{
    if (debug) fprintf(stderr, "show_fatal(\"%s\")\n", error);

    show_header(ERROR, error);
    printf("<p>");
    printf(ERROR_FATAL, WEBMASTER, mailname);
    printf("</p>\n");
    show_trailer();
    exit(0);
}


/* redirect to supplied location */
void show_location(char *location)
{
    if (debug) fprintf(stderr, "show_location(\"%s\")\n", location);

    printf("Status: 302 Found\n");
    printf("Location: %s\n\n", location);
}


/* strip control and invalid (non utf-8) characters */
char *secure(char *text)
{
    char *src, *dest;
    size_t i;

    if (debug) fprintf(stderr, "secure(\"%s\")", text);

    if (text) {
        for (src=text, dest=text; *src; /* nop */) {
            if (*src >= 0x20 && *src < 0x7f) { /* 1 byte, skip control characters and <DEL> (0x7f) */
                *dest++ = *src++;
            } else if ((*src & 0b11100000) == 0b11000000 &&     /* 2 byte prefix */
                        (*(src+1) & 0b11000000) == 0b10000000) {    /* 2nd byte valid */
                for (i=0; i<2; i++) *dest++ = *src++;
            } else if ((*src & 0b11110000) == 0b11100000 &&     /* 3 byte prefix */
                        (*(src+1) & 0b11000000) == 0b10000000 &&    /* 2nd byte valid */
                        (*(src+2) & 0b11000000) == 0b10000000) {    /* 3rd byte valid */
                for (i=0; i<3; i++) *dest++ = *src++;
            } else if ((*src & 0b11111000) == 0b11110000 &&     /* 4 byte prefix */
                        (*(src+1) & 0b11000000) == 0b10000000 &&    /* 2nd byte valid */
                        (*(src+2) & 0b11000000) == 0b10000000 &&    /* 3rd byte valid */
                        (*(src+3) & 0b11000000) == 0b10000000) {    /* 4th byte valid */
                for (i=0; i<4; i++) *dest++ = *src++;
            } else {
                src++;
            }
        }
        *dest = '\0';
    }

    if (debug) fprintf(stderr, "=\"%s\"\n", text);

    return text;
}


/* replace <CR><LF> with <LF> only */
char *stripcr(char *text)
{
    char *src, *dest;

    if (debug) fprintf(stderr, "stripcr(\"%s\")\n", text);

    if (text) {
        for (src=text, dest=text; *src; src++) {
            if (src[0] != '\r' || src[1] != '\n') {
                *dest++ = *src;
            }
        }
        *dest = '\0';
    }
    return text;
}


/* check for valid address */
char *checkaccess(char *address)
{
    FILE *f;
    char line[1024];
    char *ptr;

    if (debug) fprintf(stderr, "checkaccess(\"%s\")\n", address);

    if ((f = fopen(ADDRESSES, "r"))) {
        while (fgets(line, sizeof(line), f)) {
            if (*line != '#') {    /* skip comments */
                /* strip trailing newline */
                if ((ptr = strchr(line, '\n'))) {
                    *ptr = '\0';
                }
                if (debug) fprintf(stderr, "checkaccess: \"%s\"\n", line);
                if (strcmp(address, line) == 0) {
                    /* address in file */
                    fclose(f);
                    if (debug) fprintf(stderr, "checkaccess: valid \"%s\"\n", line);
                    return address;
                }
            }
        }
        /* address not in file */
        fclose(f);
        snprintf(line, sizeof(line), BAD_ADDRESS, address, POSTMASTER, mailname);
        show_error(line);
        return NULL;
    } else {
        show_fatal(ERROR_READ);
        return NULL;
    }
}


/* actually send mail to recipient(s) */
void mailto(char *address, char *cc, char *bcc, char *subject, char *from)
{
    FILE *p;
    int i;
    char *ptr, *nptr;
    int retval;
    char error[BUFSIZE];

    if (debug) fprintf(stderr, "mailto(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")\n", address, cc, bcc, subject, from);

    if ((p = popen(MAILCMD, "w"))) {
        fprintf(p, "HELO %s\r\n", fqdn);
        fprintf(p, "MAIL FROM:<%s@%s>\r\n", SENDER, mailname);
        fprintf(p, "RCPT TO:<%s>\r\n", address);
        if (cc) {
            fprintf(p, "RCPT TO:<%s>\r\n", cc);
        }
        if (bcc) {
            fprintf(p, "RCPT TO:<%s>\r\n", bcc);
        }
        fprintf(p, "DATA\r\n");
        /* message headers */
        fprintf(p, "From: %s\r\n", from);
        fprintf(p, "Sender: %s@%s\r\n", SENDER, mailname);
        fprintf(p, "To: %s\r\n", address);
        if (cc) {
            fprintf(p, "Cc: %s\r\n", cc);
        }
        fprintf(p, "Subject: %s\r\n", subject);
        fprintf(p, "Content-Type: text/plain; charset=utf-8\r\n");
        fprintf(p, "Content-Disposition: inline\r\n");
        fprintf(p, "Content-Transfer-Encoding: 8bit\r\n");
        if ((ptr = getenv("REMOTE_ADDR")) && *ptr) {
            fprintf(p, "X-Addr: %s\r\n", ptr);
        }
        if ((ptr = getenv("REMOTE_HOST")) && *ptr) {
            fprintf(p, "X-Host: %s\r\n", ptr);
        }
        if ((ptr = getenv("REMOTE_IDENT")) && *ptr) {
            fprintf(p, "X-Ident: %s\r\n", ptr);
        }
        if ((ptr = getenv("REMOTE_USER")) && *ptr) {
            fprintf(p, "X-User: %s\r\n", ptr);
        }
        fprintf(p, "\r\n");
        /* message body: print all the fields preceeded by their name */
        if (max > MAXFIELDS) {
            max = MAXFIELDS;
        }
        for (i = 0; i < max; i++) {
            /* tag dot-stuffing */
            if (*htag[i] == '.') {
                fprintf(p, ".");
            }
            if ((nptr = strchr(hval[i], '\n'))) {
                /* multiline format */
                fprintf(p, "%s:\r\n", htag[i]);
                ptr = hval[i];
                do {
                    /* terminate line */
                    *nptr = '\0';
                    /* line dot-stuffing */
                    if (*ptr == '.') {
                        fprintf(p, ".");
                    }
                    fprintf(p, "%s\r\n", secure(ptr));
                    /* restore newline */
                    *nptr = '\n';
                    /* get next line */
                    ptr = nptr + 1;
                } while ((nptr = strchr(ptr, '\n')));
                /* final line */
                if (*ptr == '.') {
                    fprintf(p, ".");
                }
                fprintf(p, "%s\r\n", secure(ptr));
                fprintf(p, "..\r\n");
            } else {
                /* single line format */
                fprintf(p, "%s: %s\r\n", htag[i], secure(hval[i]));
            }
        }
        /* end message */
        fprintf(p, ".\r\n");
        fprintf(p, "QUIT\r\n");
        /* check return value for success */
        retval = pclose(p);
        if (retval) {
            snprintf(error, sizeof(error), ERROR_PCLOSE, MAILCMD, retval);
            show_fatal(error);
        }
    } else {
        snprintf(error, sizeof(error), ERROR_POPEN, MAILCMD, strerror(errno));
        show_fatal(error);
    }
}


void usage(char *image)
{
    fprintf(stderr, "Usage: %s [-h] [-v]\n", image);
    exit(1);
}


/* get the fqdn of this machine */
void getfqdn(char *name, size_t len)
{
    struct addrinfo hints;
    struct addrinfo *result;

    gethostname(name, len-1);
    /* \0 not guaranteed by gethostname() if len exceeded */
    name[len-1] = '\0';
    /* get canonical name */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* allow IPv4 or IPv6 */
    hints.ai_flags = AI_CANONNAME;  /* get canonical name */
    if (getaddrinfo(name, NULL, &hints, &result) == 0) {
        /* canonical name returned in first addrinfo structure */
        if (result != NULL) {
            snprintf(name, len, "%s", result->ai_canonname);
        }
    }
    freeaddrinfo(result);
}


/* get the local mailname for later insertion */
void getmailname(char *name, size_t len, char *fallback)
{
    FILE *f;
    size_t sl;

    name[0] = '\0';

    /* try /etc/mailname first (Debian specific) */
    if ((f = fopen("/etc/mailname", "r")) != NULL) {
        if (fgets(name, len, f)) {
            /* strip trailing newline */
            sl = strlen(name);
            if (sl > 0 && name[sl-1] == '\n') {
                name[sl-1] = '\0';
            }
        }
        fclose(f);
    }
    /* fall back to supplied name */
    if (!strlen(name)) {
        snprintf(name, len, "%s", fallback);
    }
}


/* extract tokens from streams */
char *fntok_r(FILE *f, const char *delim, size_t *n)
{
    size_t len, i;
    char *token;
    char c;

    len = BUFSIZE, i = 0;
    token = malloc(len);

    while (*n > 0 && !feof(f)) {
        /* read next character */
        c = (char)fgetc(f);
        (*n)--;
        /* look for c in delimiters */
        if (strchr(delim, c)) {
            /* terminate and return token */
            token[i] = '\0';
            return token;
        } else {
            /* append character to current token */
            token[i] = c;
        }
        i++;
        /* resize token buffer if necessary */
        if (i >= len-1) {
            len += BUFSIZE;
            token[i] = '\0';
            token = realloc(token, len);
        }
    }

    token[i] = '\0';
    /* if no token remains, return NULL */
    if (*token == '\0') {
        free(token);
        token = NULL;
    }
    return token;
}


char x2c(char *x) {
    char c;

    c = (x[0] >= 'A' ? ((x[0] & 0xdf) - 'A') + 10 : (x[0] - '0'));
    c *= 16;
    c += (x[1] >= 'A' ? ((x[1] & 0xdf) - 'A') + 10 : (x[1] - '0'));

    return c;
}


void urldecode(char *url) {
    size_t i;   /* current index of input */
    size_t j;   /* current index of output, j <= i */

    for (i=0, j=0; url[i]; i++, j++) {
        if (url[i] == '+') {
            url[j] = ' ';
        } else if (url[i] == '%') {
            url[j] = x2c(&url[i+1]);
            i += 2;
        } else {
            url[j] = url[i];
        }
    }
    url[j] = '\0';
}


int main(int argc, char *argv[])
{
    int c;
    char *address, *from, *cc, *bcc, *subject, *location;
    char *ptr, *nptr;
    char *tag, *val;
    size_t cl, i;
    char *saveptr;

    while ((c = getopt(argc, argv, "Dvh?")) != EOF) {
        switch ((char)c) {
            case 'D':
                debug++;
                break;
            case 'v':
                fprintf(stderr, header);
                exit(0);
                break;
            case 'h':
                fprintf(stderr, header);
                /* fall through */
            case '?':
                usage(argv[0]);
                break;
        }
    }

    address = NULL;
    subject = NULL;
    location = NULL;
    from = SENDER;
    cc = NULL;
    bcc = NULL;

    getfqdn(fqdn, sizeof(fqdn));
    if (debug) fprintf(stderr, "fqdn=\"%s\"\n", fqdn);

    getmailname(mailname, sizeof(mailname), fqdn);
    if (debug) fprintf(stderr, "mailname=\"%s\"\n", mailname);

    /* extract recipient address from URL */
    ptr = getenv("PATH_INFO");
    if (debug) fprintf(stderr, "PATH_INFO=\"%s\"\n", ptr);
    if (ptr) {
        /* skip leading slash */
        if (*ptr == '/') {
            ptr++;
        }
        address = secure(ptr);
        if (debug) fprintf(stderr, "address=\"%s\"\n", address);
    }

    /* only accept POST requests */
    ptr = getenv("REQUEST_METHOD");
    if (debug) fprintf(stderr, "REQUEST_METHOD=\"%s\"\n", ptr);
    if (!ptr || strcmp(ptr, "POST") != 0) {
        show_fatal(ERROR_REQUEST_METHOD);
    } else {
        /* parse form content, extract special keywords */
        ptr = getenv("CONTENT_LENGTH");
        cl = ptr ? atoi(ptr) : 0;
        if (cl <= 0) {
            show_fatal(ERROR_CONTENT_LENGTH);
        }
        max = 0;
        while ((val = fntok_r(stdin, "&", &cl))) {
            if (*val) {
                urldecode(val);
                tag = secure(strtok_r(val, "=", &saveptr));
                val = strtok_r(NULL, "=", &saveptr);
                if (val) {
                    if (strcmp(tag, "To") == 0) {
                        address = secure(val);
                    } else if (strcmp(tag, "Subject") == 0) {
                        subject = secure(val);
                        if (debug) fprintf(stderr, "subject=\"%s\"\n", subject);
                    } else if (strcmp(tag, "From") == 0) {
                        from = secure(val);
                    } else if (strcmp(tag, "Cc") == 0) {
                        cc = secure(val);
                    } else if (strcmp(tag, "Bcc") == 0) {
                        bcc = secure(val);
                    } else if (strcmp(tag, "Acknowledge") == 0) {
                        location = secure(val);
                    } else {
                        /* treat htag, hval as circular buffers */
                        htag[max % MAXFIELDS] = tag;
                        hval[max % MAXFIELDS] = stripcr(val);
                        max++;
                    }
                }
            }
        }

        /* ensure address and subject supplied */
        if (!address || !*address || !subject || !*subject) {
            show_error(ERROR_ADDRESS);
        }

        /* check for valid addresses */
        address = checkaccess(address);
        cc = (cc && *cc) ? checkaccess(cc) : NULL;
        bcc = (bcc && *bcc) ? checkaccess(bcc) : NULL;

        /* send mail */
        mailto(address, cc, bcc, subject, from);

        /* redirect user to acknowledgement page */
        if (location && *location) {
            show_location(location);
        } else {
            /* generate an acknowledgement page */
            show_header(SUCCESS_HEADER, NULL);
            printf("<p>");
            printf(SUCCESS_SENT, address);
            if (cc && bcc) {
                printf(SUCCESS_ALSO, cc);
                printf(SUCCESS_COPY, bcc);
            } else if (cc) {
                printf(SUCCESS_COPY, cc);
            } else if (bcc) {
                printf(SUCCESS_COPY, bcc);
            } else {
                printf(".\n");
            }
            printf("</p>\n<p>\n");
            printf(SUCCESS_FROM, from);
            printf(SUCCESS_SENDER, SENDER, mailname);
            printf(SUCCESS_TO, address);
            if (cc) {
                printf(SUCCESS_CC, cc);
            }
            if (bcc) {
                printf(SUCCESS_BCC, bcc);
            }
            printf(SUCCESS_SUBJECT, subject);
            if ((ptr = getenv("REMOTE_ADDR")) && *ptr) {
                printf(SUCCESS_ADDR, ptr);
            }
            if ((ptr = getenv("REMOTE_HOST")) && *ptr) {
                printf(SUCCESS_HOST, ptr);
            }
            if ((ptr = getenv("REMOTE_IDENT")) && *ptr) {
                printf(SUCCESS_IDENT, ptr);
            }
            if ((ptr = getenv("REMOTE_USER")) && *ptr) {
                printf(SUCCESS_USER, ptr);
            }
            printf("</p>\n");
            for (i=0; i < max; i++) {
                if ((nptr = strchr(hval[i], '\n'))) {
                    /* multiline format */
                    printf(SUCCESS_MULTI_START, htag[i]);
                    ptr = hval[i];
                    do {
                        *nptr = '\0';
                        printf(SUCCESS_MULTI_LINE, secure(ptr));
                        ptr = nptr + 1;
                    } while ((nptr = strchr(ptr, '\n')));
                    /* final line */
                    printf(SUCCESS_MULTI_END, secure(ptr));
                }
                else {
                    /* single line format */
                    printf(SUCCESS_SINGLE, htag[i], secure(hval[i]));
                }
            }
            show_trailer();
        }
    }

    return 0;
}
