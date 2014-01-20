<% var session = Request.getSession(true); %>
Valid: <%= Request.isRequestedSessionIdValid() %>
<% if (! session.isNew()) session.invalidate(); %>
Valid: <%= Request.isRequestedSessionIdValid() %>
New: <%= session.isNew() %>

