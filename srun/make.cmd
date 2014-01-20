rem Dirty makefile replacement for building mod_caucho for OS/2

set ap_incl=/0HOLE/apache/build/apache-1.3-rus/src/include
set ap_incl_os2=/0HOLE/apache/build/apache-1.3-rus/src/os/os2
set ap_core_lib=ApacheCoreOS2.a

gcc -c -I%ap_incl% -I%ap_incl_os2% -O2 -DOS2 -DTCPIPV4 -s -Zmt mod_caucho.c
gcc -c -I%ap_incl% -I%ap_incl_os2% -O2 -DOS2 -DTCPIPV4 -s -Zmt config.c
gcc -c -I%ap_incl% -I%ap_incl_os2% -O2 -DOS2 -DTCPIPV4 -s -Zmt registry.c
gcc -c -I%ap_incl% -I%ap_incl_os2% -O2 -DOS2 -DTCPIPV4 -s -Zmt stream.c

gcc -Zmtd -Zsysv-signals -Zbin-files -Zdll -o caucho mod_caucho.o config.o registry.o stream.o %ap_core_lib% -lsocket -lbsd -lufc mod_caucho.def
emxbind -b -q -s -h0 -dmod_caucho.def caucho
rm caucho