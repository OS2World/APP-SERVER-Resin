<%@ page language=java import='jsp.*, java.util.*'
%><head>
<title>Guest Book Login</title>
</head>
<body bgcolor='white'>

<h1>Guest Book Comments</h1>

<!-- use the cached included page to display the guestbook -->
<jsp:include page='guestbook_inc.jsp'/>

<hr>

<% if (request.getAttribute("message") != null) { %>
<h1 style='color : red'><%= request.getAttribute("message") %></h1>
<% } %>

<form action='<%= response.encodeURL("GuestJsp") %>' method='post'>
<input type=hidden name='action' value='login'>
<table>
<tr><td>Name:<td><input name='Name'
                        value="<%= request.getAttribute("Name") %>">
<tr><td>Password:<td><input name='Password' type='password'>
<tr><td>Hint:<td>The password is 'quidditch'
<tr><td><input type=submit value='Login'>
</table>
</form>
</body>
