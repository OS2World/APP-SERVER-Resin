<%@ page language=javascript %>
<body bgcolor='white'>

<h1>Guest Book Comments</h1>
<table>
<tr><td width='25%'><em>Name</em><td><em>Comment</em>
<%
var list = application.attribute.guest_book;

for (var i = 0; i < list.size(); i++) {
  var item = list.get(i);
  out.writeln("<tr><td>", item.name, "<td>", item.comment);
}
%>
</table>
<hr>

<% if (request.attribute.message) { %>
<h1 style='color : red'><%= request.attribute.message %></h1>
<% } %>

<form action='GuestJsp' method='post'>
<input type=hidden name='action' value='login'>
<table>
<% if (request.attribute.Name) { %>
<tr><td>Name:<td><input name='Name' value="<%= request.attribute.Name %>">
<% } else { %>
<tr><td>Name:<td><input name='Name'>
<% } %>
<tr><td>Password:<td><input name='Password' type='password'>
<tr><td>Hint:<td>The password is 'quiddich'
<tr><td><input type=submit value='Login'>
</table>
</form>
</body>
