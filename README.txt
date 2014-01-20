Copyright (c) 1998-2000 Caucho Technology. All Rights Reserved.

This is the README file for Resin(tm)
=====================================

The documentation for starting Resin is at doc/ref/starting.html

Questions should go to our mailing list.  To subscribe, send a mail to 
resin-interest-request@caucho.com with the body "subscribe".

You can also send us your bug reports to bugs@caucho.com


I) Licensing
------------

Although Resin is Open Source, we expect companies, organizations,
and funded projects to support us by purchasing Resin licenses.
For pricing and licensing information, please see our sales page at
http://www.caucho.com/sales.html

Customers modifying or redistributing Resin, including other open
source projects, must read the license carefully to make sure your
redistribution satisfies the terms of the license.


II) Preconditions
-----------------

1) Resin needs a Java JDK before it can run.  Resin works with JDK 1.1.x
and JDK 1.2.  You need the JDK, not the JRE because Resin needs the
Java compiler.  We also recommend using IBM's jikes compiler.
Some locations for good JDKs include:

  http://java.sun.com      -- Sun's site
  http://www.ibm.com       -- IBM's fast 1.1.8 and 1.3beta JDKs for Linux
                           -- We recomment also using The jikes compiler.
  http://www.blackdown.org -- The Blackdown port for Linux..

2) On Unix, Resin's wrapper.pl script expects Perl 5.

3) On Windows, you'll need an unzip tool available at http://www.winzip.com



III) Getting Started
--------------------

Start your Resin evaluation with Resin's included httpd server
even if you plan to use Resin with another web server.

The included HTTP/1.1 server makes it easy to evaluate Resin, JSP
and servlets.  Just start it up.  The default configuration
files can be used without modification.

1) To start the server:

     unix> resin1.1/bin/httpd.sh

   or

     c:\> resin1.1/bin/httpd

   The server listens to port 8080.

2) Browse http://localhost:8080 or equivalently http://127.0.0.1:8080.

3) Go through the tutorial at http://localhost:8080/java_tut.

4) Run through the demos at http://localhost:8080/examples

5) For JSP, create test.jsp in resin1.2/doc/test.jsp and browse
   http://localhost:8080/test.jsp.

   Resin serves files from resin1.2/doc in the default configuration.
   All files with the extension '.jsp' are interpreted as jsp files.

6) For Servlets, create resin1.2/doc/WEB-INF/classes/HelloServlet.java
   and browse http://localhost:8080/servlet/HelloServlet

   Resin automatically compiles Java classes in resin1.2/WEB-INF/classes.  
   Add the servlet source in WEB-INF/classes and browse.


IV) Plugin Configuration (unix)
-------------------------------

To install for Apache or Netscape, you'll need to compile the plugins:

  configure --with-apache=/usr/local/apache \
            --with-netscape=/usr/local/netscape/https-your-hostname
  make

Full details are in doc/ref/starting.html.


V) Plugin Configuration (windows)
---------------------------------

To install Resin with IIS, Apache or Netscape, use

  c:\> resin1.1\bin\setup

Full details are in doc/ref/starting.html.

VI) XSL
-------

XSL can be run standalone as follows:

  > java com.caucho.xsl.Xsl [-xsl stylesheet] [-o dest] file1.xml file2.xml ...

By default it uses XSLT rules.  For XSLT-list, add the '-list'
flag.
