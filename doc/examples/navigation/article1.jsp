<% 
import fancy_header;

writeHeader(out, application, "Navigation Examples", request.requestURI);

writeSitemap(out, application, request.requestURI);
%>

This is a very simple and stupid article.  There's really not all that
much to say here except that this is really, really dumb.

<%
writeFooter(out, application, request.requestURI, "prevnext")
%>
