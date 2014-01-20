<%@ page import='java.util.*'
%><html>
<head><title>Environment Script</title></head>
<body bgcolor=#ffffff>

<h1>Requested URL:</h1>
<!--
  -- Java classes, including static functions are available through
  -- the special 'Packages' object. 
  -->

<pre>
<%= HttpUtils.getRequestURL(request) %>
</pre>

<!-- The Request and Response objects conform to the Servlet API -->
 
<h1>Request Information:</h1>
<table>
<tr><td>Request method      <td><%= request.getMethod() %>
<tr><td>Request URI         <td><%= request.getRequestURI() %>
<tr><td>Request protocol    <td><%= request.getProtocol() %>
<tr><td>Servlet path        <td><%= request.getServletPath() %>
<tr><td>Path info           <td><%= request.getPathInfo() %>
<tr><td>Path translated     <td><%= request.getPathTranslated() %>
<tr><td>Query string        <td><%= request.getQueryString() %>
<tr><td>Content length      <td><%= request.getContentLength() %>
<tr><td>Content type        <td><%= request.getContentType() %>
<tr><td>Server name         <td><%= request.getServerName() %>
<tr><td>Server port         <td><%= request.getServerPort() %>
<tr><td>Remote user         <td><%= request.getRemoteUser() %>
<tr><td>Remote address      <td><%= request.getRemoteAddr() %>
<tr><td>Remote host         <td><%= request.getRemoteHost() %>
<tr><td>Authorization scheme<td><%= request.getAuthType() %>
</table>

<h1>Request Headers:</h1>
<table>
<%

Enumeration e = request.getHeaderNames();
while (e.hasMoreElements()) {
  String name = (String) e.nextElement();
  %><tr><td><%= name %><td><%= request.getHeader(name) %>
  <%
}

%>
</table>

<h1>Form Values:</h1>
<table>

<%
//
// The query property returns form values.
//
e = request.getParameterNames();
while (e.hasMoreElements()) {
  String name = (String) e.nextElement();
  %><tr><td><%= name %><td><%= request.getParameter(name) %>
  <%
}

%>

</table>

</body>
</html>
