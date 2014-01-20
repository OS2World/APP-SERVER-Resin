<%@ page language=java import='java.util.*'%><%!
//
// A simple guestbook using a single JSP page and no beans.
//
// This architecture is appropriate for pages that retrieve data,
// like a database or an XML file, but don't do much with the user's
// input.
//
// As the page becomes more complicated, encapsulating data sources,
// like guestBook, into a bean will make the code more readable.
//
%>
<head>
<title>JSP Guest Book</title>
</head>

<body bgcolor='white'>
<h1>JSP Guest Book</h1>

<!-- Retrieve the guestbook from the application, creating if necessary -->
<jsp:useBean id='guestBook' class='java.util.Hashtable' scope=application/>

<!--
   - "controller" code, handling state changes and user input
  -->
<%
// If already logged in, grab the user name.
String name = (String) session.getAttribute("name");

// Try to log the user in
String user = request.getParameter("Name");
String passwd = request.getParameter("Password");

if (user == null)
  user = "";
else if ("quidditch".equals(passwd)) {
  name = user;
  session.setAttribute("name", name);
}

// Update the guestbook if the user has a new comment.
String comment = request.getParameter("Comment");
if (comment != null && name != null)
  guestBook.put(name, comment);
if (comment == null && name != null)
  comment = (String) guestBook.get(name);
if (comment == null)
  comment = "";

%>
<!--
   - display the guestbook
  -->
<table>
<tr><td width='25%'><em>Name</em></td><td><em>Comment</em></td>
<%
Enumeration e = guestBook.keys();
while (e.hasMoreElements()) {
  String guestName = (String) e.nextElement();
  String guestComment = (String) guestBook.get(guestName);

  %><tr><td><%= guestName %></td><td><%= guestComment %></td></tr><%
}
%>
</table>
<hr>

<!--
   - If the user is not logged in, show him a login form.
   - The action is encoded to allow URL-based sessions.
  -->
<% if (name != null) { %>
<form action='<%= response.encodeURL("guestbook.jsp") %>' method='POST'>
<table>
<tr><td>Name:<td><%= name %>
<tr><td>Comment:<td><input size="40" name="Comment" value="<%= comment %>">
</table>
</form>

<!-
  - Otherwise, if the user is logged in, let him change the comment.
 -->
<% } else { %>
<form action="<%= response.encodeURL("guestbook.jsp") %>" method="POST">
<table>
<tr><td>Name:<td><input name='Name' value='<%= user %>'>
<tr><td>Password:<td><input name='Password' type='password'>
<tr><td>Hint:<td>The password is 'quidditch'
<tr><td><input type=submit value='Login'>
</table>
</form>
<% } %>
</body>
