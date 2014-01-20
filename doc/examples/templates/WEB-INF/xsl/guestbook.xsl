<!--
   - guestbook.xsl converts the guestbook templates to JSP servlets.
   -
   - It defines the following special tags
   - 
   - ct:display-guest-book - display the contents of the guest book
   - ct:admin-guest-book   - create an admin table for the guest book
   - ct:error-message      - display any error message from the servlet
  -->

<xsl:stylesheet>

<xsl:output method=html disable-output-escaping='true' indent='false'/>
<!-- 
   - The stylesheet is static; it will always produce the same JSP for
   - an XTP file.  So the JSP only needs to be generated once.
  -->

<xsl:import href='defaultcopy.xsl'/>
<xsl:import href='header.xsl'/>
<xsl:import href='form.xsl'/>

/ 
<<
<%@ page import='jsp.*, java.util.*' %>
<xsl:apply-templates/>
>>

<!--
   - Displays the guest book contents in a simple table.
   -
   - The guest book is in the application attribute "guest_book"
   - The template assumes that the caller will properly synchronize the
   - guest book before invoking the servlet.
  -->
ct:guest-book
<<
<%
  {
    GuestBook _ct_guest_book;
    _ct_guest_book = (GuestBook) application.getAttribute("guest_book");
    Iterator _ct_guest_iter = _ct_guest_book.iterator();
    while (_ct_guest_iter.hasNext()) {
      Guest _ct_guest = (Guest) _ct_guest_iter.next();
%>
<xsl:apply-templates/>
<%
    }
  }
%>
>>

ct:guest-name <<
<%= _ct_guest.getName() %>
>>

ct:guest-comment <<
<%= _ct_guest.getComment() %>
>>

ct:guest-admin[@action] <<
<a href="GuestXtp?action=<{@action}>&Name=<%=
  _ct_guest.getName().replace(' ', '+')
%>"><{@action}></a>
>>

<!--
   - Displays an error message passed from the servlet to the template in
   - the request attribute "message".
  -->
ct:error-message
<<
<% if (request.getAttribute("message") != null) { %>
<h1 style='color : red'><%= request.getAttribute("message") %></h1>
<% } %>
>>

</xsl:stylesheet>
