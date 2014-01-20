<%@ page language=java import='jsp.*, java.util.*' session=false %><%
//
// This included page displays the contents of the guestbook taking, advantage
// of Resin's caching.  Because the guestbook changes infrequently, it is
// cached for 15 seconds.  A real application would cache the page for
// 15 minutes.
//
// Of course, for a hashtable-based guestbook that's not a big win, but
// is more important for a database page.
//
// Also, notice that the top-level page is not cacheable because it uses
// sessions.  Even so, the included page is cached because it doesn't use
// sessions.  The "session=false" above is very important to make caching
// work.
//
// To see a log of Resin's caching, enable the /caucho.com/http/cache log.
//

response.setDateHeader("Expires", System.currentTimeMillis() + 15000);
%>
<!-- Retrieve the guestbook from the application, creating if necessary -->
<jsp:useBean id='guest_book' class='jsp.GuestBook' scope=application/>

<!--
   - display the guestbook
  -->
<table>
<tr><td width='25%'><em>Name</em></td><td><em>Comment</em></td>
<%
Iterator iter = guest_book.iterator();
while (iter.hasNext()) {
  Guest guest = (Guest) iter.next();

  %><tr><td><%= guest.getName() %></td>
        <td><%= guest.getComment() %></td></tr><%
}
%>
</table>
