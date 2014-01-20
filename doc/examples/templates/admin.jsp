<%@ page language=javascript %>
<head>
<title>Guest Book Administration</title>
</head>
<body bgcolor='white'>

<h1>Guest Book Administration</h1>

<table width='80%'>
<tr><td width='25%'><em>Name</em><td><em>Comment</em><td><em>Action</em>
<%
var guestBook = application.attribute.guest_book;

for (var guest in guestBook) {
  out.writeln("<tr><td>", guest.name, "<td>", guest.comment,
              "<td><a href=\"GuestJsp?action=delete&Name=",
              guest.name.replace(/\s+/, "+"),
              "\">delete</a>");
}
%>
</table>
<hr>

<% if (request.attribute.message) { %>
<h1 style='color : red'><%= request.attribute.message %></h1>
<% } %>

<!-- Login as another user -->
<form action='GuestJsp' method='post'>
<input type=hidden name='action' value='login'>
<table>
<tr><td>Name:<td><input name='Name'>
<tr><td>Password:<td><input name='Password' type='password'>
<tr><td>Hint:<td>The password is 'quiddich'
<tr><td><input type=submit value='Login'>
</table>
</form>
</body>
