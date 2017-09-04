Changelog
=========

2.0.0 (2017-09-04)
------------------
* mailto now verifies all outgoing addresses against /etc/mailto.conf.
* mailto no longer supports extra data (realnames) in /etc/mailto.conf.
* Removed template mail form; only POST requests are accepted.
* Removed Copy-Self field (potential spam vector).
* Removed deprecated Reply-To field.
* Removed German translation.
* Removed Debian packaging files.
* Removed lwp-test program.
* Code and documentation updated and reformatted throughout.
* Addressed potential buffer overflow vulnerabilities.

1.3.2-3 (2010-01-16)
--------------------
* Disable included version of getline() since it is unused
* Don't use '0' flag used with '%p' gnu_printf format

1.3.2-2 (2007-03-17)
--------------------
* Extract the local mail hostname from /etc/mailname
* Fixed two compiler warnings

1.3.2-1 (2004-12-12)
--------------------
* Added support for special mail headers required when delivering mail with non-ascii character encoding

1.3.2 (1999-03-02)
------------------
* Conditional error-to.

1.3.1-1 (2004-12-11)
--------------------
* Adjusted native report on Cc
* Added section about special variables for better documentation
* Added missing \r for the smtp protocol

1.3.1 (1998-09-02)
------------------
* Fixed carbon copy option.

1.3-1 (2004-12-09)
------------------
* New license: GNU GPLv2
* Added documentation for carbon copy (Cc)
* Better note webmaster@ as address to send errors to

1.3 (1996-08-13)
----------------
* Option to send a carbon copy. (Not yet working)

1.2.7-1 (2004-12-08)
--------------------
* Fixed format string vulnerability

1.2.7 (1996-04-12)
------------------
* Added random number (pid) code to subject parsing

1.2.6-5 (2004-12-04)
--------------------
* Cleaned up some code in mailto.c
* Wrote and added a small test program
* Added an UTF-8 detection rules so that valid UTF-8 characters won't be eaten anymore

1.2.6-4 (2001-08-10)
--------------------
* Finally fixed the redirection problem that mailto had.
* Added bilingual message for HTTP relocation
* Send CRLF in the SMTP dialog instead of just LF
* Corrected some German messages
* Added information about potential mail lossage when the pipe to sendmail is closed which caused an error

1.2.6-3 (2000-01-04)
--------------------
* Don't use strip for non-strippable files

1.2.6-2 (1999-10-26)
--------------------
* Removed case-sensitivity of email addresses

1.2.6-1 (1998-09-29)
--------------------
* Added readme
* Support for SENDER/MASTER@`hostname -f`
* Support for appropriate Redirect
* Added example

1.2.6 (1996-01-30)
------------------
* Fixed address checking bug with addresses without realnames

1.2.5 (1996-01-29)
------------------
* Added debugging output
* Fixed some possible references to null-pointers

1.2.4 (1996-01-25)
------------------
* Added support for machines lacking strerror(3C)

1.2.3 (1995-09-27)
------------------
* Gateway identifies with Sender: instead of From:
* User address now specified in From: field, Reply-To alias for From

1.2.2 (1995-09-05)
------------------
* Added <TAB> to the address file separators

1.2.1 (1995-08-02)
------------------
* X- postfix for Addr, Host, Ident and User headers.

1.2 (1995-06-01)
----------------
* Option to send a copy of the generated mail to oneself.
* Fixed bug when client sends <CR><LF> instead of <LF> only.

1.1.1 (1995-01-23)
------------------
* Option to show generated mail in automatic acknowledgement page.

1.1 (1995-01-19)
----------------
* Added evaluation of INPUT fields for To, Subject and Acknowledge.
* Default FORM now generates INPUT fields instead of URL parameters.

1.0.2 (1994-08-23)
------------------
* Fixed location bug.
* Supplies default FORM on method GET now.

1.0.1 (1994-06-10)
------------------
* Fixed documentation bug.

1.0 (1994-06-10)
----------------
* Initial version
