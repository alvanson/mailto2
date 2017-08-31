/* General page layout definitions */

#define	TITLE	"WWW Mail Gateway"

/* Everything's fine */

#define	SUCCESS_HEADER	NULL
#define	SUCCESS_DESC	"Your message has been sent.\n"
#define	SUCCESS_SENT	"Your message has been sent to %s"
#define	SUCCESS_ALSO	", %s"
#define	SUCCESS_COPY	" and a copy to %s.\n"
#define	SUCCESS_FROM	"<P><CODE>From:</CODE> %s\n"
#define	SUCCESS_SUBJECT	"<BR><CODE>Subject:</CODE> %s\n"
#define	SUCCESS_SENDER	"<BR><CODE>Sender:</CODE> %s@%s\n"
#define	SUCCESS_TO	"<BR><CODE>To:</CODE> %s\n"
#define	SUCCESS_CC	"<BR><CODE>Cc:</CODE> %s\n"
#define	SUCCESS_BCC	"<BR><CODE>Bcc:</CODE> %s\n"
#define	SUCCESS_REPLYTO	"<BR><CODE>Reply-To:</CODE> %s\n"
#define	SUCCESS_ADDR	"<BR><CODE>X-Addr:</CODE> %s\n"
#define	SUCCESS_HOST	"<BR><CODE>X-Host:</CODE> %s\n"
#define	SUCCESS_IDENT	"<BR><CODE>X-Ident:</CODE> %s\n"
#define	SUCCESS_USER	"<BR><CODE>X-User:</CODE> %s\n"
#define	SUCCESS_SINGLE	"<P><CODE>%s:</CODE> %s\n"
#define	SUCCESS_MULTI	"<P><CODE>%s:</CODE>\n"
#define	SUCCESS_LINE	"<BR>%s\n"

/* Error messages */

#define	ERROR	"Error"
#define	ERROR_FATAL	"<P>This shouldn't occur. Please contact <EM>%s@%s</EM>, describing from which page you got this error and what you did enter. Your software environment could be interesting, too."
#define	ERROR_REQUEST_METHOD	"Wrong REQUEST_METHOD (%s), expected %s."
#define	ERROR_CONTENT_TYPE	"Wrong CONTENT_TYPE (%s), expected %s."
#define	ERROR_CONTENT_LENGTH	"CONTENT_LENGTH is zero."
#define	ERROR_ADDRESS	"Missing recipient or subject information."
#define	ERROR_READ	"Error accessing the addresses file."
#define	ERROR_POPEN	"Error sending the mail:<P><CODE>popen(\"%s\"): %s</CODE>"
#define	ERROR_PCLOSE	"Error sending the mail:<P><CODE>%s</CODE> returns exit code %d.  Mail could be lost."
#define	BAD_ADDRESS	"Address %s isn't listed in the addresses file. To protect our users from anonymous mails, addresses have to be registered with %s@%s first."

/* Redirection */

#define	REDIRECT	"<html>\r\n\
<body>\r\n\
The page has been moved to <a href=\"%s\">%s</a>\r\n\
</body>\r\n\
</html>\r\n\
"
