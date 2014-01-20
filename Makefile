# Generated automatically from Makefile.in by configure.
.SUFFIXES: .jsp .xtp
VERSION=1.1.4
EG_SRC=$(shell find examples -name CVS -prune -o -name \*.\?\?p -print)
PWD=$(shell pwd)
DIST=$(PWD)/resin$(VERSION)
SERVLETS=$(DIST)/doc/WEB-INF/classes
BEANS=$(DIST)/doc/WEB-INF/lib
JSP_OBJ=com/caucho/jsp com/caucho/server com/caucho/xml com/caucho/xpath \
    com/caucho/sql com/caucho/util com/caucho/java \
    com/caucho/vfs org/w3c javax/servlet org/xml com/caucho/Version.class
OBJ=$(JSP_OBJ) com/caucho/es com/caucho/eswrap com/caucho/xsl com/caucho/regexp
SRC=Makefile org javax com/caucho/Version.java com/caucho/es \
    com/caucho/eswrap com/caucho/java com/caucho/jsp com/caucho/regexp \
    com/caucho/server com/caucho/sql com/caucho/util com/caucho/vfs \
    com/caucho/xml com/caucho/xpath com/caucho/xsl
CP=.:../lib/jsdk22.jar:$(JAVA_HOME)/jre/lib/rt.jar:$(JAVA_HOME)/lib/tools.jar
XSL=xsl.sh

all	: classes subdirs

classes	:
	mkdir classes

javadoc	: TAG
	cd src; $(MAKE) doc

TAG	:

pubdoc	:
	cd src; $(MAKE) pubdoc

tags	:
	cd src; $(MAKE) tags

subdirs	:
	cd src; $(MAKE)

install	: foo
	cd src; $(MAKE) install

foo	:

dist	: dist-win dist-unix

iis	: foo
	- cp /dosc/resin1.1/src/c/plugin/iis_srun/Release/*.dll bin
	- cp /dosc/resin1.1/src/c/plugin/nsapi/Release/*.dll bin
	- cp /dosc/resin1.1/src/win32/httpd/Release/*.exe bin
	- cp /dosc/resin1.1/src/win32/srun/Release/*.exe bin
	- cp /dosc/resin1.1/src/win32/setup/Release/*.exe bin
	- cp /dosc/resin1.1/src/c/plugin/apache/apache134/*.dll srun/win32-1.3.4
	- cp /dosc/resin1.1/src/c/plugin/apache/apache136/*.dll srun/win32-1.3.6

dist-jars	: classes subdirs
	- sed "s/__VERSION__/Resin $(VERSION) -- `date`/" < src/com/caucho/RawVersion.tmpl > src/com/caucho/Version.java
	$(MAKE) subdirs
	- mkdir -p $(DIST)/lib
	(cd classes; jar cf $(DIST)/lib/resin.jar $(OBJ))
	(cp lib/jsdk22.jar $(DIST)/lib)
	(cp lib/jdk12.jar $(DIST)/lib)
	- mkdir $(DIST)/srun
	- cp srun/*.{c,h} srun/install.sh $(DIST)/srun
	chmod +x $(DIST)/srun/install.sh
	- mkdir -p $(DIST)/src/c/jni
	- cp -r src/c/jni/{Makefile*,*.c,*.h} $(DIST)/src/c/jni
	- cp -r xsl $(DIST)/xsl
	cp Makefile $(DIST)
	cp src/Makefile $(DIST)/src
	cp LICENSE README QPL $(DIST)
	- mkdir $(DIST)/conf
	cp conf/resin.conf $(DIST)/conf
	- mkdir $(DIST)/conf/samples
	cp conf/{resin,simple,servlet,iis,apache,load_balance,deploy,virtual_host,debug}.conf $(DIST)/conf/samples

dist-docs	:
	- mkdir -p $(DIST)/doc/examples
	- cp -r doc/examples/* $(DIST)/doc/examples
	$(XSL) -lite -o $(DIST) -suffix html -xsl scripts/doc.xsl `find doc -name \*.xml`
	- mkdir $(DIST)/doc/css
	- cp ../web/css/default.css $(DIST)/doc/css
	- mkdir $(DIST)/doc/images
	- cp ../web/css/{caucho,pixel,hbleed,background,smallnew}.gif $(DIST)/doc/images
	- cp ../web/images/*.gif $(DIST)/doc/images
	- rm -r $(DIST)/doc/tutorial
	- cp ../web/articles/appserver.gif $(DIST)/doc/ref
	- cp doc/java_tut/*.{xtp,xml} $(DIST)/doc/java_tut
	- mkdir -p $(DIST)/doc/examples/templates/WEB-INF/classes/jsp
	- mkdir -p $(DIST)/doc/WEB-INF/classes/com/caucho/web
	- cp -r doc/WEB-INF/classes/com/caucho/web/*.java \
             $(DIST)/doc/WEB-INF/classes/com/caucho/web
	- mkdir -p $(SERVLETS)
	- mkdir -p $(BEANS)
	- cp doc/WEB-INF/classes/{HelloServlet,Env}.java $(DIST)/doc/WEB-INF/classes
#	- mkdir $(DIST)/doc/test
#	- cp -r doc/test/* $(DIST)/doc/test

dist-javadoc	:
	cd src; $(MAKE) pubdoc
	- cp -r javadoc $(DIST)/doc

dist-unix	: dist-clean dist-jars dist-unix-copy dist-docs dist-javadoc
	- rm resin-$(VERSION).tar.gz
	- tar -zcf resin-$(VERSION).tar.gz resin$(VERSION)
	- cp resin-$(VERSION).tar.gz resin-1.1snap.tar.gz

dist-unix-copy	:
	- rm -r $(DIST)/bin
	- mkdir $(DIST)/bin
	- cp bin/{httpd,srun,xsl}.sh $(DIST)/bin
	- cp bin/wrapper.pl $(DIST)/bin
	- mkdir $(DIST)/src/c/plugin
	- cp src/c/plugin/Makefile.in $(DIST)/src/c/plugin
	- autoconf
	- automake --include-deps --foreign src/c/plugin/{common,apache,nsapi}/Makefile
	- find $(DIST)/src/c \( -name \*.o -o -name \*.lo -o -name \*.so -o \
	                        -name \*.a -o -name .libs -o -name Makefile \
                                -o -name \*.la \) -exec rm -r {} \;
	- cp configure configure.in install-sh config.guess missing mkinstalldirs config.sub ltconfig ltmain.sh $(DIST)
	- cp Makefile.in.dist $(DIST)/Makefile.in
	- rm -f $(DIST)/Makefile $(DIST)/src/Makefile
	- rm -f $(DIST)/bin/*.exe
	- rm -rf $(DIST)/srun
	- rm -rf $(DIST)/{com,work}
	- find $(DIST) \( -name CVS -o -name \*~ -o -name \#\* -o -name core \) -exec rm -rf {} 2>/dev/null \;

dist-win	: dist-clean dist-jars dist-docs dist-javadoc
	- mv $(DIST)/README $(DIST)/README.txt
	- rm -r $(DIST)/bin
	- mkdir $(DIST)/bin
	- cp bin/{*.exe,*.dll} $(DIST)/bin
	- cp -r src/c/* $(DIST)/src/c
	- find $(DIST)/src/c \( -name \*.o -o -name \*.lo -o -name \*.so -o \
	                        -name \*.a -o -name .libs -o -name Makefile \
                                -o -name \*.la \) -exec rm -r {} \;
	- rm $(DIST)/bin/{resin}.exe
	- mkdir $(DIST)/srun/win32-1.3.6
	- cp srun/win32-1.3.6/mod_caucho.dll $(DIST)/srun/win32-1.3.6
	- mkdir $(DIST)/srun/win32-1.3.4
	- cp srun/win32-1.3.4/mod_caucho.dll $(DIST)/srun/win32-1.3.4
	- rm -rf $(DIST)/{com,work}
	- find $(DIST) \( -name CVS -o -name \*~ -o -name \#\* -o -name core \) -exec rm -rf {} >2/dev/null \;
	- rm resin-$(VERSION).zip
	- zip -r resin-$(VERSION).zip resin$(VERSION)
	- cp resin-$(VERSION).zip resin-1.1snap.zip

dist-src:
	- rm -r $(DIST)
	- mkdir -p $(DIST)/src
	- cp -r src $(DIST)
	- find $(DIST)/src/c \( -name \*.o -o -name \*.lo -o -name \*.so -o \
	                        -name \*.a -o -name .libs -o -name Makefile \
                                -o -name \*.la \) -exec rm -r {} \;
	- rm -r $(DIST)/src/*.java
	- rm -r $(DIST)/src/com/caucho/{client,cover,editor,tools,test,sgml}
	- cp LICENSE QPL ChangeLog $(DIST)
	cp configure configure.in install-sh config.guess missing mkinstalldirs config.sub ltconfig ltmain.sh $(DIST)
	cp Makefile.in.dist $(DIST)/Makefile.in
	- mkdir -p $(DIST)/lib
	- cp lib/jdk12.jar $(DIST)/lib
	- cp lib/jsdk22.jar $(DIST)/lib
	- find $(DIST) \( -name CVS -o -name \*~ -o -name \#\* -o \
                          -name core \) -exec rm -rf {} \;
	- mkdir $(DIST)/srun
	- cp srun/*.{c,h} srun/srun.{mak,dep,rc} srun/install.sh $(DIST)/srun
	chmod +x $(DIST)/srun/install.sh
	- mkdir $(DIST)/bin
	- cp bin/{xsl,httpd,srun}.sh $(DIST)/bin
	- cp bin/wrapper.pl $(DIST)/bin
	- cp bin/{*.exe,*.dll} $(DIST)/bin
	- rm $(DIST)/bin/{resin,setup}.exe
	- mkdir $(DIST)/srun/win32-1.3.6
	- cp srun/win32-1.3.6/mod_caucho.dll $(DIST)/srun/win32-1.3.6
	chmod +x $(DIST)/bin/httpd.sh
	cp ChangeLog LICENSE README $(DIST)
	cp resin.conf $(DIST)
	- rm resin-$(VERSION)-src.zip
	- tar -zcf resin-$(VERSION)-src.tar.gz resin$(VERSION)
	- zip -r resin-$(VERSION)-src.zip resin$(VERSION)
	- cp resin-$(VERSION)-src.tar.gz resin-1.1snap-src.tar.gz
	- cp resin-$(VERSION)-src.zip resin-1.1snap-src.zip

dist-clean	:
	- rm -rf $(DIST)

clean	:
	- rm -rf classes $(DIST)
	- rm -rf log
	- find . -name core -exec rm {} \;
	(cd src; make clean)

spotless	: clean
	find . \( -name core -o -name \*~ -a -type f \) -exec rm {} \;

