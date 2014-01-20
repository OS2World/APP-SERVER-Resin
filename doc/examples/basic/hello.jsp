<%@ page session=false %>
<%
//
// Scripts between the scriptlet tags are simply executed.
//
String title = "Hello, world!";
%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<head>
<meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">

<!-- The expression tags interpolate script variables into the HTML -->

<title><%= title %></title>
</head>

<body bgcolor=white>

<h1><%= title %></h1>

</body>
