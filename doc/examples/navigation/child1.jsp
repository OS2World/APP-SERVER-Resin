<%@ page language=javascript %><%
import header;

writeHeader(out, application, "XML Table of Contents", request.requestURI);
%>

<h1>XML Table of Contents</h1>

<p>The navigation is controlled by a simple XML file sitting in each
directory.   The XML file describes the table of contents for the
directory and thus the navigation.  The table of contents for this
directory is in <a href='../view_source.jsp?navigation/toc.xml'>toc.xml</a>

<p>This child is intentionally plain to simplify the example.
<%
writeFooter(out)
%>
