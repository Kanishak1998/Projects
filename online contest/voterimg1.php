<?php
	  include 'vheader.php';
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
  background-color: rgb(133, 193, 233   );
}


div.tble
{
	margin-left: 220px;
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

                       	<tr><td ><button><a href="test.php?a='.$n.'">vote</button></td></tr></table></td>';  
      echo"</tr>";
        }  
        ?>  
    </table>
</div>
        </table>  
       </body>
       </html>