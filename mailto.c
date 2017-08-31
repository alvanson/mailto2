/*
** Mailto v1.3.2
**
** A WWW FORMs to Mail gateway
**
** Copyright (c) 1994-1999 by Andreas Ley <ley@rz.uni-karlsruhe.de>
** Copyright (c) 1998,2004 by Martin Schulze <joey@infodrom.org>
**
** Permission to use, copy, modify, and distribute this software for any
** purpose and without fee is hereby granted, provided that the above
** copyright notice appears in all copies. This software is provided "as is"
** and without any express or implied warranties.
**
** This program has been tested on a HP9000/715 with HP-UX A.09.05
** In this environment, neither lint -u nor gcc -Wall produce any messages.
** If you encounter any errors or need to make any changes to port it
** to another platform, please contact me.
**
** Usage:
**    - Since WWW is anonymous, mail sent through the WWW Mail Gateway
**	is anonymous, too. Some people don't like to get anonymous mail,
**	that's why mailto can be used with an access configuration file,
**	which specifies who is enabled to get mail through the WWW Mail
**	Gateway. If you don't need access control, just undefine ADDRESSES,
**	otherwise point it to a configuration file with entries:
**		<address> (<realname>)
**	<realname> is optional.
**    - Calling /cgi-bin/mailto/<address>/<subject>/<url> gives you a template
**	mail FORM. <address> is the recipients email address, <subject> is
**	a predefined subject and <url> is the URL of an acknowledgement page.
**	Any of the parameters may be skipped; since address and subject
**	are required, mailto will generate input fields if they're missing.
**    - Create your own customized/beautified FORM with METHOD=POST and
**	ACTION=/cgi-bin/mailto/<address>/<subject>/<url>. As above, <address>
**	is the recipients email address, <subject> is a predefined subject and
**	<url> is the URL of an acknowledgement page. You can replace any of
**	these parameters by INPUTs with NAME= "To", "Subject" or "Acknowledge"
**	respectivly, which may also be of TYPE=hidden (recommended for the
**	acknowledgement URL). These INPUTs will be treated special, as will
**	be NAME="From" (NAME="Reply-To" provides the same functionality but
**	is obsolete and it's use is discouraged), NAME="Cc" and
**	NAME="Copy-Self". They all have precedence over the URL parameters. Use
**	of the INPUTs is recommended.
**	When the FORM is submitted, the <address> (or the VALUE of the
**	NAME="To" field) will be checked against the configuration file, the
**	contents will be converted into a mail message and the message will be
**	sent to <address> (<realname>) with <subject> (or the VALUE of the
**	NAME="Subject" field) as the subject. Input fields will be listed as
**	"<name>: <content><LF>" for one-line-contents and as
**	"<name>:<LF><content><LF><content><LF>.<LF>" for multi-line-contents
**	with leading periods preceeded by another period.
**	If there is a field with NAME="Cc", a carbon copy of the mail message
**	will be sent to the addresses listed in its VALUE.
**	If an INPUT with TYPE="checkbox" and NAME="Copy-Self" has been checked,
**	a copy of the message will be sent to the originator of the message
**	as specified in the NAME="From" (or NAME="Reply-To") field.
**	If <url> (or a field with NAME="Acknowledge") was specified, it will
**	be sent back as a confirmation to the sender.
**
** Version history
**
** Version 1.3.2 - 2.3.1999
**	Conditional error-to.
**
** Version 1.3.1 - 2.9.1998
**	Fixed carbon copy option.
**
** Version 1.3 - 13.8.96
**	Option to send a carbon copy. (Not yet working)
**
** Version 1.2.7 - 12.4.96
**	Added random number (pid) code to subject parsing
**
** Version 1.2.6 - 30.1.96
**	Fixed address checking bug with addresses without realnames
**
** Version 1.2.5 - 29.1.96
**	Added debugging output
**	Fixed some possible references to null-pointers
**
** Version 1.2.4 - 25.1.96
**	Added support for machines lacking strerror(3C)
**
** Version 1.2.3 - 27.9.95
**	Gateway identifies with Sender: instead of From:
**	User address now specified in From: field, Reply-To alias for From
**
** Version 1.2.2 - 5.9.95
**	Added <TAB> to the address file separators
**
** Version 1.2.1 - 2.8.95
**	X- postfix for Addr, Host, Ident and User headers.
**
** Version 1.2 - 1.6.95
**	Option to send a copy of the generated mail to oneself.
**	Fixed bug when client sends <CR><LF> instead of <LF> only.
**
** Version 1.1.1 - 23.1.95
**	Option to show generated mail in automatic acknowledgement page.
**
** Version 1.1 - 19.1.95
**	Added evaluation of INPUT fields for To, Subject and Acknowledge.
**	Default FORM now generates INPUT fields instead of URL parameters.
**
** Version 1.0.2 - 23.8.94
**	Fixed location bug.
**	Supplies default FORM on method GET now.
**
** Version 1.0.1 - 10.6.94
**	Fixed documentation bug.
**
** Version 1.0 - 10.6.94
**	Initial version
**
** Thanx for bug reports, ideas and fixes to
**	anr@ime.usp.br (Adriano R.)
**	pall@rz.uni-karlsruhe.de (Michael Pall)
**	otisg@cobalt.middlebury.edu (Otis Gospodnetic)
**	joey@infodrom.org (Martin Schulze)
**
** A current version of mailto can be retrieved from
**	ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/cgi-src/mailto.tar.gz
*/

char header[]="mailto v1.3.2\n(c) 1994-1999 by Andreas Ley <Andreas.Ley@rz.uni-karlsruhe.de>\n";

/* General definitions */

#define	MASTER	"root"  /* we add the local host part */
#define	SENDER	"not-for-mail"
#define ERRORS_TO	"webmaster"
#define	SENDMAIL_SMTP	/* There's no other method yet */
#define	FULL_ACKNOWLEDGE	/* Define this if you want an automatic full acknowledgement page */
/* #define	NEED_STRERROR	/ * Define this if you don't have strerr */

/* File locations */

#define	ADDRESSES	"/etc/mailto.conf"

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

#ifdef SENDMAIL_SMTP
#define	MAILCMD	"/usr/lib/sendmail -bs >/dev/null"
#endif /* SENDMAIL_SMTP */

#ifdef NEED_STRERROR
extern int	sys_nerr;
extern char	*sys_errlist[];
#define	strerror(errno)	((errno)>=sys_nerr?"Undefined error":sys_errlist[errno])
#endif /* NEED_STRERROR */

#define	FALSE	0		/* This is the naked Truth */
#define	TRUE	1		/* and this is the Light */

/* Functions from util.c */

char *makeword(char *line, char stop);
char *fmakeword(FILE *f, char stop, int *len);
void unescape_url(char *url);
void plustospace(char *str);

/* Global variables */

char	master[]=MASTER;
char    hostname[255];
char	*htag[1024],*hval[1024];
int	max,debug=0;


#define ENC_8859_1	"ÄäÖöÜüßÅåÆæø"

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
	(void)printf("<LINK REV=\"made\" HREF=\"mailto:%s\">\n",master);
	(void)printf("</HEAD>\n");
	(void)printf("<BODY>\n");
	(void)printf("<H1>%s</H1>\n",subtitle?subtitle:TITLE);
	if (description) {
		(void)printf(description,master,hostname);
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
	(void)printf(ERROR_FATAL,master,hostname);
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



char *checkaccess(char *address,int check)
{
	char	buffer[1024],*ptr;
	int	len;
	FILE	*src;

	if (debug)
		(void)fprintf(stderr,"checkaccess(\"%s\",%d)\n",address,check);
#ifdef ADDRESSES
	/* Check for valid address, extract full address (with personal name) */
	if ((src=fopen(ADDRESSES,"r"))) {
		len=strlen(address);
		while(fgets(buffer,sizeof(buffer),src))
			if (*buffer!='#') {
				if ((ptr=strchr(buffer,'\n')))
					*ptr='\0';
				if (debug)
					(void)fprintf(stderr,"checkaccess: \"%s\"\n",buffer);
				if ((!strncasecmp(buffer,address,(size_t)len))&&(buffer[len]==' '||buffer[len]=='\t'||buffer[len]=='\0')) {
					(void)fclose(src);
					if (debug)
						(void)fprintf(stderr,"checkaccess: valid \"%s\"\n",buffer);
					return(strdup(buffer));
				}
			}
		(void)fclose(src);
		if (check) {
			(void)sprintf(buffer,BAD_ADDRESS,address,master,hostname);
			show_error(buffer);
		}
		else
			return(address);
	}
	else
		show_fatal(ERROR_READ);
	/*NOTREACHED*/
#endif /* ADDRESSES */
	return(address);
}


char *encoding()
{
  int cnt;
  char *cp;
  static char enc[10];

  for (cnt=0; cnt<max; cnt++)
    for (cp=hval[cnt]; *cp; cp++) {
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
      if ((*cp&0xE0)>=0xC0 &&
	  *(cp+1) && (*(cp+1) & 0xC0) == 0x80){
	strcpy(enc, "utf-8");
	return enc;
      } else if ((*cp&0xD0)>=0xD0 &&
		 *(cp+1) && (*(cp+1) & 0xC0) == 0x80 &&
		 *(cp+2) && (*(cp+2) & 0xC0) == 0x80) {
	strcpy(enc, "utf-8");
	return enc;
      } else if ((*cp&0xF0)>=0xF0 &&
		 *(cp+1) && (*(cp+1) & 0xC0) == 0x80 &&
		 *(cp+2) && (*(cp+2) & 0xC0) == 0x80 &&
		 *(cp+3) && (*(cp+3) & 0xC0) == 0x80) {
	strcpy(enc, "utf-8");
	return enc;
      } else if ((*cp&0xF8)>=0xF8 &&
		 *(cp+1) && (*(cp+1) & 0xC0) == 0x80 &&
		 *(cp+2) && (*(cp+2) & 0xC0) == 0x80 &&
		 *(cp+3) && (*(cp+3) & 0xC0) == 0x80 &&
		 *(cp+4) && (*(cp+4) & 0xC0) == 0x80) {
	strcpy(enc, "utf-8");
	return enc;
      } else if ((*cp&0xFC)>=0xFC &&
		 *(cp+1) && (*(cp+1) & 0xC0) == 0x80 &&
		 *(cp+2) && (*(cp+2) & 0xC0) == 0x80 &&
		 *(cp+3) && (*(cp+3) & 0xC0) == 0x80 &&
		 *(cp+4) && (*(cp+4) & 0xC0) == 0x80 &&
		 *(cp+5) && (*(cp+5) & 0xC0) == 0x80) {
	strcpy(enc, "utf-8");
	return enc;
      } else if ((*cp&0x7f)>=0x20 &&
		 strchr(ENC_8859_1, *cp) != NULL) {
	strcpy(enc, "iso-8859-1");
	return enc;
      }
    }

  return NULL;
}


/* Actually send mail to recipient */
void mailto(char *address, char *fulladdress, char *cc, char *bcc, char *subject, char *from, int copyself)
{
	char	buffer[1024],error[2048],*ptr,*nptr;
	int	cnt,retval;
	FILE	*dest;
	char	*enc;

	if (debug)
		(void)fprintf(stderr,"mailto(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d)\n",address,fulladdress,cc,bcc,subject,from,copyself);
#ifdef SENDMAIL_SMTP
	strcpy(buffer,MAILCMD);
	if ((dest=popen(buffer,"w"))) {
		(void)fprintf(dest,"HELO localhost\r\n");
		(void)fprintf(dest,"MAIL FROM:<%s@%s>\r\n",SENDER,hostname);
		(void)fprintf(dest,"RCPT TO:<%s>\r\n",address);
		if (cc)
			(void)fprintf(dest,"RCPT TO:<%s>\r\n",cc);
		if (bcc)
			(void)fprintf(dest,"RCPT TO:<%s>\r\n",bcc);
		if (copyself)
			(void)fprintf(dest,"RCPT TO:<%s>\r\n",from);
		(void)fprintf(dest,"DATA\r\n");
		(void)fprintf(dest,"From: %s\r\n",from);
		(void)fprintf(dest,"Subject: %s\r\n",subject);
		(void)fprintf(dest,"Sender: %s@%s\r\n",SENDER,hostname);
#ifdef ERRORS_TO
		(void)fprintf(dest,"Errors-To: %s@%s\r\n",ERRORS_TO,hostname);
#endif
		(void)fprintf(dest,"To: %s\r\n",fulladdress);
		if ((enc = encoding()) != NULL) {
			(void)fprintf(dest,"Content-Type: text/plain; charset=%s\r\n", enc);
			(void)fprintf(dest,"Content-Disposition: inline\r\n");
			(void)fprintf(dest,"Content-Transfer-Encoding: 8bit\r\n");
		}
		if (cc)
			(void)fprintf(dest,"Cc: %s\r\n",cc);
		if ((ptr=getenv("REMOTE_ADDR"))&&*ptr)
			(void)fprintf(dest,"X-Addr: %s\r\n",ptr);
		if ((ptr=getenv("REMOTE_HOST"))&&*ptr)
			(void)fprintf(dest,"X-Host: %s\r\n",ptr);
		if ((ptr=getenv("REMOTE_IDENT"))&&*ptr)
			(void)fprintf(dest,"X-Ident: %s\r\n",ptr);
		if ((ptr=getenv("REMOTE_USER"))&&*ptr)
			(void)fprintf(dest,"X-User: %s\r\n",ptr);
		(void)fprintf(dest,"\r\n");
		/* Print all the fields preceeded by their name */
		for (cnt=0;cnt<max;cnt++) {
			/* Names starting with a period must be escaped with another period */
			if (*htag[cnt]=='.')
				(void)fprintf(dest,".");
			/* Multiline values have a different format... */
			if (strchr(hval[cnt],'\n')) {
				(void)fprintf(dest,"%s:\r\n",htag[cnt]);
				ptr=hval[cnt];
				while ((nptr=strchr(ptr,'\n'))) {
					*nptr='\0';
					if (*ptr=='.')
						(void)fprintf(dest,"..");
					(void)fprintf(dest,"%s\r\n",secure(ptr));
					*nptr++='\n';
					ptr=nptr;
				}
				(void)fprintf(dest,"%s\r\n",secure(ptr));
				(void)fprintf(dest,"..\r\n");
			}
			/* ...while singlelines are just Name: Value */
			else
				(void)fprintf(dest,"%s: %s\r\n",htag[cnt],secure(hval[cnt]));
		}
		/* Ok, end of transmission */
		(void)fprintf(dest,".\r\n");
		(void)fprintf(dest,"QUIT\r\n");
		retval=pclose(dest);
		if (retval) {
			(void)sprintf(error,ERROR_PCLOSE,MAILCMD,retval);
			show_fatal(error);
		}
	}
	else {
		(void)sprintf(error,ERROR_POPEN,buffer,strerror(errno));
		show_fatal(error);
	}
#endif
}



void usage(char *image)
{
	(void)fprintf(stderr,"Usage: %s [-h] [-v]\n",image);
	exit(1);
}


int main(int argc, char *argv[])
{
	int	c;
	char	*ptr,*nptr;
	char	*address,*fulladdress,*from,*cc,*bcc,*subject,*location;
	char	*tag,*val;
	int	cl,cnt,copyself;
	char	localhost[100];
	struct	hostent *hent;
	FILE	*f;

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
	copyself=FALSE;

	/* Get the local hostname for later insertion */
	hostname[0] = '\0';

	/* /etc/mailname is Debian specific */
	if ((f = fopen ("/etc/mailname", "r")) != NULL) {
	  if (fgets (hostname, sizeof(hostname), f)) {
	    if (hostname[strlen(hostname)-1] == '\n')
	      hostname[strlen(hostname)-1] = '\0';
	  } else
	    hostname[0] = '\0';
	  fclose (f);
	}

	if (!strlen(hostname)) {
	gethostname(localhost, sizeof(localhost));
	if ( index(localhost, '.')) {
		snprintf(hostname, sizeof(hostname), "%s", localhost);
	} else {
		hent = gethostbyname(localhost);
		if ( hent )
			snprintf(hostname, sizeof(hostname), "%s", hent->h_name);
		else
			snprintf(hostname, sizeof(hostname), "%s", localhost);
	}
	}

	ptr=getenv("PATH_INFO");
	if (debug)
		(void)fprintf(stderr,"PATH_INFO=\"%s\"\n",ptr);
	if (ptr) {
		if (*ptr=='/')
			ptr++;
		if (*ptr) {
			address=ptr;
			if ((subject=strchr(address,'/'))) {
				*subject++='\0';
				if ((location=strchr(subject,'/'))) {
					*location++='\0';
					if ((ptr=strchr(location,':'))&&ptr[1]=='/') {
						if (ptr[2]!='/') {
							nptr=malloc(strlen(location)+2);
							(void)strncpy(nptr,location,(size_t)(ptr-location)+2);
							(void)strcpy(nptr+(ptr-location)+2,ptr+1);
							location=nptr;
						} else if (ptr[3]=='/')
							(void)strcpy(ptr+2,ptr+3);
					}
				}
			}
		}
		if (address&&!*secure(address))
			address=NULL;
		if (subject&&!*secure(subject))
			subject=NULL;
		if (location&&!*secure(location))
			location=NULL;
	}

	/* In case we get a GET request, spit out a minimal mail form */
	ptr=getenv("REQUEST_METHOD");
	if (debug)
		(void)fprintf(stderr,"REQUEST_METHOD=\"%s\"\n",ptr);
	if (!ptr||strcmp(ptr,"POST")) {
		show_header(NULL,NULL);
		(void)printf(DEFAULT_FORM_HEADER,"/cgi-bin/mailto");
		(void)printf(address?DEFAULT_FORM_HIDDEN:DEFAULT_FORM_ADDRESS,"To",address);
		(void)printf(DEFAULT_FORM_CC,"Cc");
		(void)printf(subject?DEFAULT_FORM_HIDDEN:DEFAULT_FORM_SUBJECT,"Subject",subject);
		if (location)
			(void)printf(DEFAULT_FORM_HIDDEN,"Acknowledge",location);
		(void)printf(DEFAULT_FORM_TRAILER);
		show_trailer();
	}

	else {
		/* Sometimes this doesn't work - due to broken clients?
		char	buffer[1024];
		if (strcmp(getenv("CONTENT_TYPE"),"application/x-www-form-urlencoded")) {
			(void)sprintf(buffer,ERROR_CONTENT_TYPE,getenv("CONTENT_TYPE"),"application/x-www-form-urlencoded");
			show_fatal(buffer);
		}
		*/

		/* Parse form, extract special keywords */
		cl=atoi(getenv("CONTENT_LENGTH"));
		if (cl==0)
			show_fatal(ERROR_CONTENT_LENGTH);
		max=0;
		while (cl&&(!feof(stdin))) {
			val=fmakeword(stdin,'&',&cl);
			plustospace(val);
			unescape_url(val);
			tag=secure(makeword(val,'='));
#ifdef FLEMMING
			if (*val) {
#endif
				if (!strcmp(tag,"To")) {
					if (*secure(val))
						address=val;
				} else if (!strcmp(tag,"Subject")) {
					if (*secure(val)) {
						subject=val;
						if (debug)
							(void)fprintf(stderr,"subject=\"%s\"\n",subject);
					}
				} else if (!strcmp(tag,"From")||!strcmp(tag,"Reply-To")) {
					if (*secure(val))
						from=val;
				} else if (!strcmp(tag,"Cc")) {
					if (*secure(val))
						cc=val;
				} else if (!strcmp(tag,"Bcc")) {
					if (*secure(val))
						bcc=val;
				} else if (!strcmp(tag,"Copy-Self")) {
					copyself=TRUE;
				} else if (!strcmp(tag,"Acknowledge")) {
					if (*secure(val))
						location=val;
				} else {
					if (*stripcr(val)) {
						htag[max]=tag;
						hval[max]=val;
						max++;
					}
				}
#ifdef FLEMMING
			}
#endif
		}

		if (address&&(ptr=strchr(address,' ')))
			*ptr='\0';

		/* Test for required values */
		if (!address||!subject)
			show_error(ERROR_ADDRESS);

		/* Self copy needs email address */
		if (!strcmp(from,"not-for-mail"))
			copyself=FALSE;

		/* Add pid to subject */
		if (strstr(subject,"%d")) {
			val = strstr(subject,"%d");
			ptr=malloc(strlen(subject)+16);
			cnt = strlen(subject)+16;
			*val = 0;
			val += 2;
			(void)snprintf(ptr,cnt,"%s%d%s",subject,getpid(),val);
			if (debug)
				(void)fprintf(stderr,"Subject: \"%s\" -> \"%s\"\n",subject,ptr);
			subject=ptr;
		}

		/* Send mail */
		fulladdress=checkaccess(address,TRUE);
		mailto(address,fulladdress,cc,bcc,subject,from,copyself);

		/* Drop the user a message */
		if (location)
			show_location(location);
		else {
			show_header(SUCCESS_HEADER,NULL);
			if (copyself||cc||bcc) {
				(void)printf(SUCCESS_SENT,fulladdress);
				if (copyself) {
					if (cc)
						(void)printf(SUCCESS_ALSO,checkaccess(cc,FALSE));
					if (bcc)
						(void)printf(SUCCESS_ALSO,checkaccess(bcc,FALSE));
					(void)printf(SUCCESS_COPY,checkaccess(from,FALSE));
				}
				else
					if (bcc) {
						if (cc)
							(void)printf(SUCCESS_ALSO,checkaccess(cc,FALSE));
						(void)printf(SUCCESS_COPY,checkaccess(bcc,FALSE));
					}
					else
						(void)printf(SUCCESS_COPY,checkaccess(cc,FALSE));
			}
			else
				(void)printf(SUCCESS_DESC);
#ifdef FULL_ACKNOWLEDGE
			(void)printf(SUCCESS_FROM,from);
			(void)printf(SUCCESS_SUBJECT,subject);
			(void)printf(SUCCESS_SENDER,SENDER,hostname);
			(void)printf(SUCCESS_TO,fulladdress);
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
