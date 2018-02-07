<!--#include file="conn.asp"-->
<%
dim rs,strSQL,id
set rs=server.createobject("ADODB.recordset")
id = request("id")
strSQL ="select * from news where ID=" & id
rs.open strSQL,conn,1,3
strsql=rs("news")
response.write strsql
rs.close
%>