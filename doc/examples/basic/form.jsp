<%@ page language=javascript %><%!
//
// Store the name in the session variable.  The session.jsp example
// will pick it up and display the user's name.
//
// Note: the first access to the session variable must come before writing
// any html so the web server can spit out the headers.
//
if (request.form.name)
  session.value.name = request.form.name
%>
<html>
<head><title>Form Fill</title></head>
<body bgcolor=#ffffff>

<h1>Form Values:</h1>
<table>

<%
//
// Form values (query) can be iterated and accessed by names.
// Form values are always arrays, which easily convert to strings
// with the array join function.
//
for (name in request.form) {
  %><tr><td><b><%= name %></b><td><%= request.form[name].join(" ") %>
  <%
}

%>
</table>

</body>
</html>
