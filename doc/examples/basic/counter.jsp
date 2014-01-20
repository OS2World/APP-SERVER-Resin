<%@ page %><%
//
// The counter is stored in the Application object.
// Since Application is shared between different server
// threads, it needs to be synchronized to properly update
// the state.
//
Integer count = null;
synchronized (application) {
  count = (Integer) application.getAttribute("basic.counter");
  if (count == null)
    count = new Integer(0);
  count = new Integer(count.intValue() + 1);
  application.setAttribute("basic.counter", count);
}

%>

<html>
<head><title>Counter</title></head>
<body bgcolor=#ffffff>

<h1>Welcome, visitor: <%= count %></h1>

</body>
</html>
