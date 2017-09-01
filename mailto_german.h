/* General page layout definitions */

#define	TITLE	"WWW Mail Gateway"

/* Everything's fine */

#define	SUCCESS_HEADER	NULL
#define	SUCCESS_SELF	"Ihre Meldung wurde an %s und eine Kopie an %s abgeschickt."
#define	SUCCESS_ALSO	", %s"
#define	SUCCESS_COPY	" und eine Kopie an %s abgeschickt.\n"
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

#define	ERROR	"Fehler"
#define	ERROR_FATAL	"<P>Ein solcher Fehler darf eigentlich nicht auftreten. Bitte setzen Sie sich mit <EM>%s@%s</EM> in Verbindung und erl&auml;utern Sie, wie es zu diesem Fehler gekommen ist. Interessant ist insbesondere, von welcher Seite Sie diese Fehlermeldung erhalten, was Sie eingegeben haben und welche Software Sie verwenden."
#define	ERROR_REQUEST_METHOD	"Es wurde ein falscher REQUEST_METHOD-Aufruf (%s) verwendet, erwartet wurde %s."
#define	ERROR_CONTENT_TYPE	"Es wurde ein falscher CONTENT_TYPE (%s) &uuml;bergeben, erwartet wurde %s."
#define	ERROR_CONTENT_LENGTH	"CONTENT_LENGTH ist null."
#define	ERROR_ADDRESS	"Es fehlen die Angaben &uuml;ber Mail-Adresse und Subject."
#define	ERROR_READ	"Fehler beim Lesen der Adressdatei."
#define	ERROR_POPEN	"Fehler beim Versenden der Meldung:<P><CODE>popen(\"%s\"): %s</CODE>"
#define	ERROR_PCLOSE	"Fehler beim Versenden der Meldung:<P><CODE>%s</CODE> liefert exit code %d.  Mail kann verloren gegangen sein."
#define	BAD_ADDRESS	"Die angegebene Adresse %s ist nicht in der Liste der erlaubten Adressen eingetragen. Um die Benutzer vor anonymen Mails zu sch&uuml;tzen, m&uuml;ssen potentielle Empf&auml;nger sich (via Email an %s@%s) in diese Liste eintragen lassen."
