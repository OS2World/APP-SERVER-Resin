@echo off

rem JSDK and Resin libraries
set classpath=%classpath%;..\lib\jdk12.jar
set classpath=%classpath%;..\lib\jsdk22.jar
set classpath=%classpath%;..\lib\resin.jar

rem Definition of JVM locale
rem set lang=ru_RU.KOI8_R

java com.caucho.server.http.RunnerServer -conf ..\conf\resin.conf %1 %2 %3 %4