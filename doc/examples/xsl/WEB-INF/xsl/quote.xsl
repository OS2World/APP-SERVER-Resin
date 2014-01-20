<xtp:directive.page language='javascript'/>
<!-- Bring in the header -->
<xsl:import href='header.xsl'/>

<#!
// caches HTML pages. 
var quotes = {};

/**
 * Scan the quote.com generated HTML file for the field.
 *
 * This uses XQL search patterns.
 *
 * A nice XML output would make this simple, but quote.com creates ugly
 * (and illegal) HTML.  Still, with Resin's XQL support, extracting the
 * data is straightforward.
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
#>

<!-- 
   - Grab a quote field.
   -
   - <quote symbol='sym' field='fieldname'/>
   -
   - The HTML is cached on a per-page basis so we don't keep bugging 
   - quote.com
  -->

quote <#
     var symbol = node.attribute.symbol;
     var field = node.attribute.field;

     if (! symbol)
       throw new java.lang.Exception("quote needs symbol attribute");
     if (! field)
       throw new java.lang.Exception("quote needs field attribute");

     var url = "http://mach.quote.com/fq/fq_quote.asp?symbols=" + symbol;
     var html = quotes[symbol];
     if (! html) {
       // Need the loose parser because quote.com uses questionable HTML
       var html = caucho.xml.Html.parseFile(File(url));
       quotes[symbol] = html;
       var os = File("stdout:").openWrite();
       html.printHtml(os);
     }

     var value = getValue(html, field);

     if (value && field == "Change:") {
       // take the change sign from the % Change
       var per_change = getValue(html, "% Change");

       if (per_change.charAt(0) == '-')
         value = '-' + value;
       else
         value = '+' + value;
     }

     out.write(value);
#>
