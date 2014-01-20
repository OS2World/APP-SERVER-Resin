<%@ page language=java import='jsp.*'%><%!
//
// Rewriting the guestbook using beans and a cached included page to
// display the contents.
//
// This architecture is suitable for most display pages.  When the
// calculations and state machines become more complicated, the
// full-blown template architecture will be more appropriate.
//
// The cached included page architecture is appropriate when the top page
// is user specific but a subpage is the same for all users and is slow
// dynamic page like a database query.
//
// For this example, creating a separate bean to handle the guestbook
// is overkill, but illustrates how a more complicated application would
// start encapsulating its data.
//
%>
<head>
<title>JSP Guest Book</title>
</head>

<body bgcolor='white'>
<h1>JSP Guest Book</h1>

<!--
   - Retrieve the guestbook from the application, creating if necessary
   - GuestBook is a custom bean, living in templates/WEB-INF/classes.
  -->
<jsp:useBean id='guest_book' class='jsp.GuestBook' scope=application/>

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
  guest_book.addGuest(name, comment);

// Try to get the old comment
if (comment == null && name != null) {
  Guest guest = (Guest) guest_book.getGuest(name);
  if (guest != null)
    comment = guest.getComment();
}

if (comment == null)
  comment = "";

%>
<!--
   - Display the guestbook.  Because guestbook_inc is cached, it will only
   - be updated every 15 seconds, but the outer page is updated immediately.
  -->
<jsp:include page='guestbook_inc.jsp'/>
<hr>

<!--
   - If the user is not logged in, show him a login form.
   - The action is encoded to allow URL-based sessions.
  -->
<% if (name != null) { %>
<form action='<%= response.encodeURL("guestbook_bean.jsp") %>' method='POST'>
<table>
<tr><td>Name:<td><%= name %>
<tr><td>Comment:<td><input size="40" name="Comment" value="<%= comment %>">
</table>
</form>

<!-
  - Otherwise, if the user is logged in, let him change the comment.
 -->
<% } else { %>
<form action="<%= response.encodeURL("guestbook_bean.jsp") %>" method="POST">
<table>
<tr><td>Name:<td><input name='Name' value='<%= user %>'>
<tr><td>Password:<td><input name='Password' type='password'>
<tr><td>Hint:<td>The password is 'quidditch'
<tr><td><input type=submit value='Login'>
</table>
</form>
<% } %>
</body>
