/* General page layout definitions */

#define TITLE "WWW Mail Gateway"

/* Everything's fine */

#define SUCCESS_HEADER NULL
/* Confirmation message */
#define SUCCESS_SENT "Your message has been sent to %s"
#define SUCCESS_ALSO ", %s"
#define SUCCESS_COPY " and a copy to %s."
/* Headers */
#define SUCCESS_FROM "<code>From:</code> %s\n"
#define SUCCESS_SUBJECT "<br><code>Subject:</code> %s\n"
#define SUCCESS_SENDER "<br><code>Sender:</code> %s@%s\n"
#define SUCCESS_TO "<br><code>To:</code> %s\n"
#define SUCCESS_CC "<br><code>Cc:</code> %s\n"
#define SUCCESS_BCC "<br><code>Bcc:</code> %s\n"
#define SUCCESS_REPLYTO "<br><code>Reply-To:</code> %s\n"
#define SUCCESS_ADDR "<br><code>X-Addr:</code> %s\n"
#define SUCCESS_HOST "<br><code>X-Host:</code> %s\n"
#define SUCCESS_IDENT "<br><code>X-Ident:</code> %s\n"
#define SUCCESS_USER "<br><code>X-User:</code> %s\n"
/* Body */
#define SUCCESS_SINGLE "<p><code>%s:</code> %s</p>\n"
#define SUCCESS_MULTI_START "<p><code>%s:</code>\n"
#define SUCCESS_MULTI_LINE "<br>%s\n"
#define SUCCESS_MULTI_END "<br>%s</p>\n"

/* Error messages */

#define ERROR "Error"
#define ERROR_FATAL "This shouldn't occur. Please contact <em>%s@%s</em>, describing: the page that led to this error, any data you may have entered, and your software environment."
#define ERROR_REQUEST_METHOD "Wrong REQUEST_METHOD. Expected POST."
#define ERROR_CONTENT_LENGTH "CONTENT_LENGTH is zero."
#define ERROR_ADDRESS "Missing recipient or subject information."
#define ERROR_READ "Error accessing the addresses file."
#define ERROR_POPEN "Error sending the mail:</p><p><code>popen(\"%s\"): %s</code>"
#define ERROR_PCLOSE "Error sending the mail:</p><p><code>%s</code> returns exit code %d.  Mail could be lost."
#define BAD_ADDRESS "Address %s isn't listed in the addresses file. To protect our users from anonymous mails, addresses have to be registered with %s@%s first."
