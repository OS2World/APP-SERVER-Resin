<%!
  /*
   * There are no expected exceptions, because we have no
   * File I/O or database access.  So if something goes wrong it's because
   * there's a bug in the code.
   */
%>

<html><title>TicTacToe: Badness</title>
<body><h1>TicTacToe: Badness</h1>
Something severely bad happened: <%= exception %>
<pre>
<% exception.printStackTrace(out); %>
</pre>
</body></html>
