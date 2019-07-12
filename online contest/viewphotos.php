<?php  
include 'pheader.php';
      include 'includes/dbh.inc.php';
 ?>  
<html>
<body>
<style>
body
{
 text-align: center;
}

table.main{
  border: 2px solid black;
  background-color: rgb(133, 193, 233  );
}


div.tble
{
	margin-left: 320px;
	width:10px;
}


td {
  padding: 15px;
}
td
{

  vertical-align: bottom;
}
table.sm
{
	margin-bottom:150px;  
}
tr.vote
{
	margin-left: 220px;
}
p.c
{
  margin-left: 270px;
  font-family: arial;
  font-size: 40px;
  color: #111;
  line-height: 50px;  
  background-color: lightblue;
}
</style> 
<p class="man"><br></p>
<div class="tble">
                <p Class="c"><b>YourImages</b></p> 
                <br >  
        <table class="main">  
        <?php  
        $query = "SELECT * FROM tbl_images ORDER BY id DESC";  
        $result = mysqli_query($conn, $query);  

        while($row = mysqli_fetch_array($result))  
        {  
        	if($name==$row['pid'])
        	{
			        	echo'<tr class="mtr">';
			             echo '  
			                   
			                       <td>  
			                            <img src="data:image/jpeg;base64,'.base64_encode($row['name'] ).'" height="400" width="700" class="img-thumnail" />  
                             </td>
			                       ';  
			      echo'</tr>';
			     }
        }  
        ?>  
    </table>
</div>
        </table>  
       </body>
       </html>