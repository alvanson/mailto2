These are the sources to the CGI scripts developed and used at the
University of Karlsruhe, Germany. This is work in progress, so beware
and use at your own risk. Most of the programs won't be useful without
modification, as they represent the local status. However, if you have
patches or modifications you think could be useful for others too, feel
free to contact webmaster@rz.uni-karlsruhe.de

The scripts usually constist of a single C file (with site-dependent
#defines at the top) <tool>.c, a language- and site-dependent include
file <tool>_<language>.c (only German versions available now), linked
with CGI tools, Pattern Matching tools and Network Connection tools.
There often is a script update-<tool>, which is called from the main
update script which is run by cron nightly. Sometimes there's also
another C program/script used to preprocess data.

This is the first time I distribute these programs, so may be I missed
something, forgot to include a file, etc. Please contact me if this should
be the case.

Also, if you find this useful and want to install it at your Web site,
send me a notification - I want to see if there's public interest in
these tools; if it's worth to officially release and support them.

util.c has been stolen from NCSA's CGI-Scripts distribution.
wildmat.c has been stolen from Rich Salz' INN library.
get_tcp_conn.h has been stolen from nntp.1.5.11 (?)

mailto		WWW Mail Gateway, replaces mailto: links since they don't work
		in every browser. Has a configuration file for 'allowed'
		mail addresses.
		Demo: http://www.rz.uni-karlsruhe.de/cgi-bin/mailto

Bye, Andy

-------------------------------------------------------------------------------
   Andreas Ley       !  "Even when you're   !  Email: ley@rz.uni-karlsruhe.de 
   Nelkenstr. 9      !   a genius, life is  !  Voice: +49 721 84 10 36
   76135 Karlsruhe   !   a mystery!"        !  RZ:    +49 721 608 4038
   Germany           !  Doogie Howser, M.D. !  AKK:   +49 721 96 403 22
