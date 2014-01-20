<title>Login Example</title>
<body bgcolor=white>

<h3>You are: <%
java.security.Principal user = request.getUserPrincipal();
if (user != null)
  out.println(user);
else
  out.println("nobody important");
%></h3>

This example illustrates using the form-based servlet 2.2 security
configuration.
Security is configured by
<a href='../view_source.jsp?login/WEB-INF/web.xml'>web.xml</a>.

<p>The site has control over the authentication database.  You can create
your own custom
<a href='../view_source.jsp?login/WEB-INF/classes/test/TestAuthenticator.java'>
authenticator class</a>.

<p>If you try to access a <a href='protected.jsp?a=b'>protected page</a>, you
should be presented with a login form.  If you succeed in the login, you'll
be able to see the page.

<p>Sites that use login to provide personalization can use optional login.
Fill in the following form to see this page with your name:

<form action='j_security_check' method='POST'>
<table>
<tr><td>User: <td><input name='j_username'><br>
<tr><td>Password (quidditch): <td><input name='j_password' type='password'><br>
<tr><td><input type=submit><td>
</table>

<!-- When the user logs in, direct him back to this page. -->
<input name='j_uri' type='hidden' value='index.jsp'>
</form>

</body>

