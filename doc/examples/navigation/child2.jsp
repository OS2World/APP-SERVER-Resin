<%@ page language=javascript %><%
import fancy_header;

writeHeader(out, application, "Fancy Child", request.requestURI);
%>

The only difference between the <a href='../view_source.jsp?navigation/child1.jsp'>plain child</a>
and the <a href='../view_source.jsp?navigation/child2.jsp'>fancy child</a> is the 
choice of header templates.

<p>The <a href='../view_source.jsp?navigation/WEB-INF/scripts/header.js'>plain header</a> just
defines the three column layout and the family navigation bar at left.

<p>The <a href='../view_source.jsp?navigation/WEB-INF/scripts/fancy_header.js'>fancy header</a>
adds background images, the logo and the gradient behind the title.

<p>Notice in the fancy header, that the URI for the images and stylesheet are
selected by the <em>top</em> attribute of the
<a href='../view_source.jsp?navigation/toc.xml'>table
of contents</a>.  We could have put the stylesheet name in the XML file as
well, so the style of all pages in whole directory can be changed just
by editing the XML file.

<%
writeFooter(out)
%>
