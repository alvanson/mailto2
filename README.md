# This project is no longer maintained.

mailto2
=======

mailto2 is a CGI WWW form to mail gateway written in C.  It protects against spam by allowing outgoing mail to only those addresses specified in the configuration file.  It can be used to process contact forms on otherwise static sites.

mailto2 is a fork of the Debian GNU/Linux revised version of Andreas Ley's mailto program.

Installation
------------

Run `make` to build.

Run `make install` (as root) to install to `/usr/lib/cgi-bin`.

Usage
-----

Add the email addresses that should receive mail from the WWW mail gateway to `/etc/mailto.conf`, one per line. Only mail to addresses listed in `/etc/mailto.conf` will be delivered.

Create your an HTML form with `action="/cgi-bin/mailto/<address>"` and `method="post"`.  `<address>` is the recipient's email address.  You can instead provide this parameter with a (hidden) `input` tag with `name="To"`, which will have precedence over the URL-supplied parameter.  Provide a subject with an `input` tag with `name="Subject"`.

When the form is submitted, the recipient address(es) (including Cc & Bcc recipients) will be checked against the configuration file, the contents will be converted into a mail message and sent.  Other than the special fields listed below, input fields will be listed in the message body as `<name>: <content><LF>` for single line contents and as `<name>:<LF><content><LF><content><LF>.<LF>` for multiline contents.  Leading periods will be preceded by a second period (dot-stuffing).  If a field with `name="Acknowledge"` was provided, the user will be forwarded to the specified URL upon success as a confirmation to the sender.

### Special fields

The following fields have a special meaning when they are part of the form data.  The field names are case sensitive.  They won't end up in the mail body but will be used for constructing and delivering the resulting mail.

* **From** - Submitter address, will be used as `From:` header **(required)**
* **To** - Primary recipient, can also be submitted in URL as `.../<address>`
* **Cc** - Carbon copy recipient (included in mail header)
* **Bcc** - Blind carbon copy recipient (not included in mail header)
* **Subject** - Mail subject **(required)**
* **Acknowledge** - URL for redirection on success

### Example form

```html
<form action="/cgi-bin/mailto/you@example.com" method="POST">
  <p>From: <input type="text" name="From" /></p>
  <p>Subject: <input type="text" name="Cc" /></p>
  <p>Message: <input type="text" type="Message" /></p>
  <input type="hidden" name="Acknowledge" value="/special/mailto-answer.html" />
  <button type="submit">Send</button>
</form>
```

Contributing
------------

All contributions (e.g. issue reports, pull requests, questions and comments) are encouraged.

Authors
-------

* **Andreas Ley** (ley@rz.uni-karlsruhe.de) - initial author
* **Martin Schulze** (joey@infodrom.org) - Debian package
* **Evan Thompson** ([alvanson](https://github.com/alvanson)) - current maintainer

This is Differentiated Analytics Inc.'s fork of the Debian GNU/Linux prepackaged version of Andreas Ley's mailto program.

The Debian package was put together by Martin Schulze (joey@debian.org), from sources obtained from: ftp://ftp.rz.uni-karlsruhe.de/pub/net/www/tools/cgi-src/mailto.tar.gz

One mirror still seems to exist: http://www-ftp.lip6.fr/ftp/pub/www/more-tools/cgi-src/mailto.tar.gz

Both sites only contained mailto up to version 1.2.6.

The upstream packages for 1.2.7, 1.3, 1.3.1 and 1.3.2 were sent to Joey via private mail and not released to the abovely mentioned Internet address.

Version 1.3.2 is also the final upstream version which only exists on a tape library archive and isn't in use anymore.

License
-------

* Copyright (c) 1994-1999 by Andreas Ley (ley@rz.uni-karlsruhe.de)
* Copyright (c) 1998,2004 by Martin Schulze (joey@infodrom.org)
* Copyright (c) 2017 by Differentiated Analytics Inc. (info@differentiatedanalytics.ca)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

Acknowledgements
----------------

Thank you to the following for bug reports, ideas and fixes for the original mailto project and Debian package:

* Adriano R. (anr@ime.usp.br)
* Michael Pall (pall@rz.uni-karlsruhe.de)
* Otis Gospodnetic (otisg@cobalt.middlebury.edu)
* Martin Schulze (joey@infodrom.org)
* Ryan Hayle (ryan@savvynet.com)
