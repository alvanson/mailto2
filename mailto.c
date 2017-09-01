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

char header[]="mailto2 20170831\n"
"(c) 1994-1999 by Andreas Ley <Andreas.Ley@rz.uni-karlsruhe.de>\n"
"(c) 1998,2004 by Martin Schulze <joey@infodrom.org>\n"
"(c) 2017 by Differentiated Analytics Inc. <info@differentiatedanalytics.ca>\n";

/* General definitions */

#define POSTMASTER "postmaster"
#define WEBMASTER "webmaster"
#define SENDER "www"
#define	FULL_ACKNOWLEDGE	/* Define this if you want an automatic full acknowledgement page */

/* File locations */

#define ADDRESSES "/etc/mailto.conf"
#define MAILCMD "/usr/lib/sendmail -bs >/dev/null"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
/*
#include "mailto_german.h"
*/
#include "mailto_english.h"

#define	FALSE	0		/* This is the naked Truth */
#define	TRUE	1		/* and this is the Light */

/* Global variables */

#define BUFSIZE 8192
#define MAXFIELDS 1024

char fqdn[256];
char mailname[256];
char *htag[MAXFIELDS], *hval[MAXFIELDS];
size_t max;
int	debug=0;


/* Prints header of a HTML-Request */
void show_header(char *subtitle, char *description)
{
	char	*ptr;

	if (debug)
		(void)fprintf(stderr,"show_header(\"%s\",\"%s\")\n",subtitle,description);
	while ((ptr=strchr(header,'\n')))
		*ptr=' ';
	(void)printf("Content-type: text/html\n\n");
	(void)printf("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">\n");
	(void)printf("<!-- %s-->\n",header);
	(void)printf("<HTML>\n");
	(void)printf("<HEAD>\n");
	(void)printf("<TITLE>%s%s%s</TITLE>\n",TITLE,subtitle?" - ":"",subtitle?subtitle:"");
	(void)printf("</HEAD>\n");
	(void)printf("<BODY>\n");
	(void)printf("<H1>%s</H1>\n",subtitle?subtitle:TITLE);
	if (description) {
		(void)printf(description,WEBMASTER,mailname);
		(void)printf("<P>\n");
	}
}



/* Prints trailer of a HTML-Request */
void show_trailer()
{
	if (debug)
		(void)fprintf(stderr,"show_trailer()\n");
	(void)printf("</BODY>\n");
	(void)printf("</HTML>\n");
}



/* Shows an error page, based on wrong user input */
void show_error(char *error)
{
	if (debug)
		(void)fprintf(stderr,"show_error(\"%s\")\n",error);
	show_header(ERROR,error);
	show_trailer();
	exit(0);
}



/* Shows an error page, based on internal misfunction */
void show_fatal(char *error)
{
	if (debug)
		(void)fprintf(stderr,"show_fatal(\"%s\")\n",error);
	show_header(ERROR,error);
	(void)printf(ERROR_FATAL,WEBMASTER,mailname);
	(void)printf("<P>\n");
	show_trailer();
	exit(0);
}



/* Returns a Location: header */
void show_location(char *location)
{
	if (debug)
		(void)fprintf(stderr,"show_location(\"%s\")\n",location);

	(void)printf("Content-type: text/html\n");
	(void)printf("Content-length: %ld\n", (long) (strlen (REDIRECT)-4+2*strlen (location)));
	(void)printf("Status: 302 Temporal Relocation\n");
	(void)printf("Location: %s\n\n", location);

	(void)printf(REDIRECT, location, location);
}



int tagindex(char **htag, int max, char *tagname)
{
	int	cnt;

	if (debug)
		(void)fprintf(stderr,"tagindex(%p,%08x,\"%s\")\n",htag,max,tagname);
	for (cnt=0;cnt<max&&strcmp(htag[cnt],tagname);cnt++);
	return(cnt);
}



char *secure(char *text)
{
	char	*src,*dest;
	unsigned short i;

	if (debug)
		(void)fprintf(stderr,"secure(\"%s\")\n",text);
	if (text) {
		for (src=text,dest=text;*src;)
			  /*
			   * Linkname: Table demonstrating the UTF-8 encoding
			   *      URL: http://www1.tip.nl/~t876506/utf8tbl.html
			   *
			   *   If z is between and including 0 - 127, then there is 1 byte
			   *
			   *   If z is between and including 192 - 223, then there are 2 bytes
			   *
			   *   If z is between and including 224 - 239, then there are 3 bytes
			   *
			   *   If z is between and including 240 - 247, then there are 4 bytes
			   *
			   *   If z is between and including 248 - 251, then there are 5 bytes
			   *
			   *   If z is 252 or 253, then there are 6 bytes
			   *
			   *   If z = 254 or 255 then there is something wrong!
			   */
			if ((*src&0xE0)>=0xC0 &&
			    *(src+1) && (*(src+1) & 0xC0) == 0x80)
				for (i=0;i<2;i++)
					*dest++=*src++;
			else if ((*src&0xD0)>=0xD0 &&
				 *(src+1) && (*(src+1) & 0xC0) == 0x80 &&
				 *(src+2) && (*(src+2) & 0xC0) == 0x80)
				for (i=0;i<3;i++)
					*dest++=*src++;
			else if ((*src&0xF0)>=0xF0 &&
				 *(src+1) && (*(src+1) & 0xC0) == 0x80 &&
				 *(src+2) && (*(src+2) & 0xC0) == 0x80 &&
				 *(src+3) && (*(src+3) & 0xC0) == 0x80)
				for (i=0;i<4;i++)
					*dest++=*src++;
			else if ((*src&0xF8)>=0xF8 &&
				 *(src+1) && (*(src+1) & 0xC0) == 0x80 &&
				 *(src+2) && (*(src+2) & 0xC0) == 0x80 &&
				 *(src+3) && (*(src+3) & 0xC0) == 0x80 &&
				 *(src+4) && (*(src+4) & 0xC0) == 0x80)
				for (i=0;i<5;i++)
					*dest++=*src++;
			else if ((*src&0xFC)>=0xFC &&
				 *(src+1) && (*(src+1) & 0xC0) == 0x80 &&
				 *(src+2) && (*(src+2) & 0xC0) == 0x80 &&
				 *(src+3) && (*(src+3) & 0xC0) == 0x80 &&
				 *(src+4) && (*(src+4) & 0xC0) == 0x80 &&
				 *(src+5) && (*(src+5) & 0xC0) == 0x80)
				for (i=0;i<6;i++)
					*dest++=*src++;
			else if ((*src&0x7f)>=0x20)
				*dest++=*src++;
			else
				src++;
		*dest='\0';
	}
	if (debug)
		(void)fprintf(stderr,"secure()=%p\n",text);
	return(text);
}



/* Replaces <CR><LF> by <LF> only */
char *stripcr(char *text)
{
	char	*src,*dest;

	if (debug)
		(void)fprintf(stderr,"stripcr(\"%s\")\n",text);
	if (text) {
		for (src=text,dest=text;*src;src++)
			if (src[0]!='\r'||src[1]!='\n')
				*dest++=*src;
		*dest='\0';
	}
	return(text);
}


/* check for valid address */
char *checkaccess(char *address)
{
    FILE *f;
    char line[256];
    char *ptr;

    if (debug)
        fprintf(stderr, "checkaccess(\"%s\")\n", address);

    if ((f = fopen(ADDRESSES, "r"))) {
        while (fgets(line, sizeof(line), f)) {
            if (*line != '#') {    /* skip comments */
                /* strip trailing newline */
                if ((ptr = strchr(line, '\n'))) {
                    *ptr = '\0';
                }
                if (debug)
                    fprintf(stderr, "checkaccess: \"%s\"\n", line);
                if (strcmp(address, line) == 0) {
                    /* address in file */
                    fclose(f);
                    if (debug)
                        fprintf(stderr,"checkaccess: valid \"%s\"\n",line);
                    return address;
                }
            }
        }
        /* address not in file */
        fclose(f);
        sprintf(line, BAD_ADDRESS, address, POSTMASTER, mailname);
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

    if (debug)
        fprintf(stderr, "mailto(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")\n", address, cc, bcc, subject, from);

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
	(void)fprintf(stderr,"Usage: %s [-h] [-v]\n",image);
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
	int	c;
	char	*ptr,*nptr;
	char	*address,*from,*cc,*bcc,*subject,*location;
	char	*tag,*val;
    size_t cl;
    char *saveptr;
	int	cnt;

	while ((c=getopt(argc,argv,"Dvh?")) != EOF)
		switch ((char)c) {
		case 'D':
			debug++;
			break;
		case 'v':
			(void)fprintf(stderr,header);
			exit(0);
		case 'h':
			(void)fprintf(stderr,header);
		case '?':
			usage(argv[0]);
		}

	address=NULL;
	subject=NULL;
	location=NULL;
	from="not-for-mail";
	cc=NULL;
	bcc=NULL;

    getfqdn(fqdn, sizeof(fqdn));
    if (debug)
        fprintf(stderr, "fqdn=\"%s\"\n", fqdn);

    getmailname(mailname, sizeof(mailname), fqdn);
    if (debug)
        fprintf(stderr, "mailname=\"%s\"\n", mailname);

    /* extract recipient address from URL */
    ptr = getenv("PATH_INFO");
    if (debug)
        fprintf(stderr, "PATH_INFO=\"%s\"\n", ptr);
    if (ptr) {
        /* skip leading slash */
        if (*ptr == '/') {
            ptr++;
        }
        address = secure(ptr);
        if (debug)
            fprintf(stderr, "address=\"%s\"\n", address);
    }

    /* only accept POST requests */
    ptr = getenv("REQUEST_METHOD");
    if (debug)
        fprintf(stderr, "REQUEST_METHOD=\"%s\"\n", ptr);
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
                        if (debug)
                            fprintf(stderr, "subject=\"%s\"\n", subject);
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

		/* Drop the user a message */
		if (location)
			show_location(location);
		else {
			show_header(SUCCESS_HEADER,NULL);
			if (cc||bcc) {
				(void)printf(SUCCESS_SENT,address);
					if (bcc) {
						if (cc)
							(void)printf(SUCCESS_ALSO,cc);
						(void)printf(SUCCESS_COPY,bcc);
					}
					else
						(void)printf(SUCCESS_COPY,cc);
			}
			else
				(void)printf(SUCCESS_DESC);
#ifdef FULL_ACKNOWLEDGE
			(void)printf(SUCCESS_FROM,from);
			(void)printf(SUCCESS_SUBJECT,subject);
			(void)printf(SUCCESS_SENDER,SENDER,mailname);
			(void)printf(SUCCESS_TO,address);
			if (cc)
				(void)printf(SUCCESS_CC,cc);
			if (bcc)
				(void)printf(SUCCESS_BCC,bcc);
			if ((ptr=getenv("REMOTE_ADDR"))&&*ptr)
				(void)printf(SUCCESS_ADDR,ptr);
			if ((ptr=getenv("REMOTE_HOST"))&&*ptr)
				(void)printf(SUCCESS_HOST,ptr);
			if ((ptr=getenv("REMOTE_IDENT"))&&*ptr)
				(void)printf(SUCCESS_IDENT,ptr);
			if ((ptr=getenv("REMOTE_USER"))&&*ptr)
				(void)printf(SUCCESS_USER,ptr);
			for (cnt=0;cnt<max;cnt++) {
				if (strchr(hval[cnt],'\n')) {
					(void)printf(SUCCESS_MULTI,htag[cnt]);
					ptr=hval[cnt];
					while ((nptr=strchr(ptr,'\n'))) {
						*nptr++='\0';
						(void)printf(SUCCESS_LINE,secure(ptr));
						ptr=nptr;
					}
					(void)printf(SUCCESS_LINE,secure(ptr));
				}
				else
					(void)printf(SUCCESS_SINGLE,htag[cnt],secure(hval[cnt]));
			}
#endif /* FULL_ACKNOWLEDGE */
			show_trailer();
		}
	}

	return(0);
}
