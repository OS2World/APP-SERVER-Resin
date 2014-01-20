<%@ page language=javascript %><% 
import fancy_header;

writeHeader(out, application, "Example: Navigation", request.requestURI);

writeSitemap(out, application, request.requestURI);
%>

<p>This example's navigation is automatically generated by the Resin
pages.  The active pages also generate the site decoration.

<p><a href='../view_source.jsp?navigation/navigation.jsp'>This page</a> shows off all the navigation
tricks in the example:
<ul>
<li>The family navigation at left
<li>The table of contents above
<li>The threaded (prev/next) at the bottom.
<li>And using a <a
href='../view_source.jsp?navigation/WEB-INF/scripts/fancy_header.js'>site decoration</a>
template for the colors and bleeds. 
</ul>

<%
writeFooter(out, application, request.requestURI, "prevnext");
%>
