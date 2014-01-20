<%@ page language=java session=false import='com.caucho.server.http.*' %>
<%
  Statistics stats;
  stats = (Statistics) application.getAttribute("caucho.statistics");
%>
<body bgcolor=white>

Live sessions: <%= stats.getLiveSessions() %><br>
Slow threads: <%= stats.getSlowThreads() %>

<p>
Active Threads <%= stats.getMinuteThreads() %> (minute)<br>
Active Threads <%= stats.getHourThreads() %> (hour)<br>
Active Threads <%= stats.getDayThreads() %> (day)

<p>
CPU Load <%= stats.getMinuteCpu() %> (minute)<br>
CPU Load <%= stats.getHourCpu() %> (hour)<br>
CPU Load <%= stats.getDayCpu() %> (day)<br>

</body>