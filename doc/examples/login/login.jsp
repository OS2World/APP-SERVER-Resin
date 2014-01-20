<!-- Need to encode the action in case the use has disabled cookies -->
<form action='<%= response.encodeURL("j_security_check") %>' method='POST'>
<table>
<tr><td>User:<td><input name='j_username'>
<tr><td>Password:<td><input name='j_password'>
<tr><td colspan=2>hint: the password is 'quidditch'
<tr><td><input type=submit>
</table>

<!--
   - In case the user got here without a session, redirect successful
   - requests to the index.  (This is a non-standard, but useful field.)
  -->
<input type='hidden' name='j_uri' value='/index.jsp'/>
</form>