<?php
	  include 'aheader.php';
      include 'includes/dbh.inc.php';
?>
<html>
<body>
<style>
body
{
  background-color: lightblue;
 text-align: center;
}

table.main{
  background-color: rgb(133, 193, 233  );
}


div.tble
{
	margin-left: 230px;
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
</style>
 
<br>
<br>
<p style="text-align: center;font-size: 30px;">Uploaded Photos</p>
<div class="tble">
        <table class="main">

        <?php  
        $query = "SELECT * FROM tbl_images ORDER BY id DESC";  
        $result = mysqli_query($conn, $query);  
        
        while($row = mysqli_fetch_array($result))  
        {  
        	$n=$row["id"];
        	echo'<tr class="mtr">';
             echo ' 
                   
                        <td>  
                            <img src="data:image/jpeg;base64,'.base64_encode($row['name'] ).'" height="350" width="600" class="img-thumnail" />  </td>
                        <td> 
                        	<table class="sm"><tr><td><b>Photographer_name:'.$row["pid"].'</b></td></tr>





          
                   
                      
                          

                        <tr><td ><button><a href="delete.php?a='.$n.'">Delete</button></td></tr></table></td>'; 
      echo"</tr>";
        }  
        ?>  
    </table>
</div>
        </table>  
       </body>
       </html>