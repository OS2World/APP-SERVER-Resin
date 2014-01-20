<%@ page language=java import='jsp.*, java.util.*' %>
<head>
<title>Guest Book Add Comment</title>
</head>
<body bgcolor='white'>

<h1>Guest Book Add Comment</h1>

<!-- use the cached included page to display the guestbook -->
<jsp:include page='guestbook_inc.jsp'/>

<hr>

<% if (request.getAttribute("message") != null ) { %>
<h1 style='color : red'><%= request.getAttribute("message") %></h1>
<% } %>

<form action='<%= response.encodeURL("GuestJsp") %>' method='post'>
<input type=hidden name='action' value='add'>
<table>
<tr><td>Name:<td><%= session.getValue("name") %>

<tr><td>Comment:
    <td><input name='Comment' size=40
               value="<%= request.getAttribute("Comment") %>">

<tr><td><input type=submit value='Add Comment'>
</table>
</form>
</body>
