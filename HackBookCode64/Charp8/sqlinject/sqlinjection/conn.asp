<%
dim dbpath,conn
dbpath=server.MapPath("/data/data.mdb")
set conn=server.CreateObject("adodb.connection")
conn.open "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & dbpath
%>