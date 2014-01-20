<%@ page language=javascript %>
<!-- <%@ error=quote_error.jsp %> -->
<%
/**
 * Scan the HTML file for the field.
 *
 * This uses XQL search patterns.
 *
 * Granted, the pattern is ugly, but the HTML is ugly.  If quote.com
 * produced a nice XML output, this routine would be clearer.
 */
function getValue(doc, field)
{
  var row = doc.find('//tr[td/font/text()="' + field + '"]');

  if (row) {
    var iter = row.select("td/font/text()");
    iter.next();
    var value = iter.next();
    return value.nodeValue;
  } else
    return null;
}

/**
 * Returns a quote object parsed from quote.com
 */

function getQuote(symbol)
{
  var url = "http://fast.quote.com/fq/quotecom/quote?symbols=" + symbol;

  // Need the loose parser because quote.com uses questionable HTML
  var doc = caucho.sgml.LooseHtmlParser.parseFile(File(url));

  // take the change sign from the % Change
  var per_change = getValue(doc, "% Change");
  var sign = '+';
  if (per_change.charAt(0) == '-')
    sign = '-';

  var quote = {
    symbol : symbol,
    high : getValue(doc, "Today's High"),
    low : getValue(doc, "Today's Low"),
    open : getValue(doc, "Open"),
    last : getValue(doc, "Last Sale:"),
    per_change : getValue(doc, "% Change"),
    change : sign + getValue(doc, "Change:"),
  }

  return quote;
}

/**
 * Gather quote information before displaying it.
 */

var sunw = getQuote("sunw");
var ibm = getQuote("ibm");
var intc = getQuote("intc");

%>

<table border=3>
<tr><th>Symbol<th>Open<th>Low<th>High<th>Last<th>Change<th>% Change

<tr><td>SUNW
    <td><%= sunw.open %>
    <td><%= sunw.low %>
    <td><%= sunw.high %>
    <td><%= sunw.last %>
    <td><%= sunw.change %>
    <td><%= sunw.per_change %>

<tr><td>IBM
    <td><%= ibm.open %>
    <td><%= ibm.low %>
    <td><%= ibm.high %>
    <td><%= ibm.last %>
    <td><%= ibm.change %>
    <td><%= ibm.per_change %>

<tr><td>INTC
    <td><%= intc.open %>
    <td><%= intc.low %>
    <td><%= intc.high %>
    <td><%= intc.last %>
    <td><%= intc.change %>
    <td><%= intc.per_change %>
</table>

