<%@ page language=javascript %>
<body bgcolor='white'>
<h1>Previous Comments</h1>

<table>
<tr><td width='25%'><em>Name</em><td><em>Comment</em>
<%
var list = request.attribute.list;

for (var i = 0; i < list.size(); i++) {
  var item = list.get(i);
  out.writeln("<tr><td>", item.name, "<td>", item.comment);
}
%>
</table>
<hr>

<% if (! session.value.name) { %>
<form action='GuestJsp' method='post'>
<input type=hidden name='action' value='login'>
<table>
<tr><td>Name:<td><input name='name'>
<tr><td>Password:<td><input size=40 name='password' type='password'>
<tr><td>Hint:<td>The password is 'quiddich'
<tr><td><input type=submit value='Login'>
</table>
</form>
<% } else { %>
<form action='GuestJsp' method='post'>
<input type=hidden name='action' value='add'>
<table>
<tr><td>Name:<td><%= session.value.name %>
<tr><td>Comment:<td><input size=40 name='comment'>
<tr><td><input type=submit value='Add Comment'>
</table>
</form>
</body>
<% } %>
</body>
