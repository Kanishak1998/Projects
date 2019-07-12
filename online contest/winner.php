	<?php
	  include 'aheader.php';

      include 'includes/dbh.inc.php';
	?>
<style>
body
{
	
  background-color: lightblue;
	text-align: center;
}
p.ma
{

  font-family: arial;
  font-size: 30px;
  color: #111;
  line-height: 50px;
  background-color: lightblue;
}
p.head{
  font-family: arial;
  font-size: 20px;
  color: #111;
  line-height: 50px;  
  background-color: lightblue;

}
}
table, th, td {
    border: 1px solid black;
    font-family: arial;
  font-size: 500px;
  color: #111;
  line-height: 50px;  
  background-color: orange;
  width: 100%;
  padding: 15px;

}
tr
{

  width: 100%;
}
tr:hover {background-color: #f5f5f5;}
th {
  background-color: #4CAF50;
  color: white;
}
div.tble
{
	margin-left: 600px;
}
</style>
	<section>
		
			<p class="man"><br></p>
			<p class="ma"><b>Admin</b></p>

			<p class="ma">Winner</p>
			<div class="tble" >
<table class="main">  
				
        <?php  
        $query = "SELECT * FROM tbl_images ORDER BY vcount DESC";  
        $row = mysqli_fetch_assoc(mysqli_query($conn, $query));  
        $name=$row['pid'];
        $query1= "SELECT * FROM user WHERE NAME='$name'";  
        $row1 = mysqli_fetch_assoc(mysqli_query($conn, $query1));  
        echo'<tr class="mtr">';
			            echo '  
			            	<td>  
			                        Photo_id:
                            </td>
			                <td>  
			                        '.$row['id'].'
                            </td>
                            
                            ';  

			echo'</tr>';
			 echo'<tr class="mtr">';
			            echo '  
			            	<td>  
			                        Photo_NAME:
                            </td>
			                <td>  
			                        '.$row['pid'].'
                            </td>
                            
                            ';  

			echo'</tr>';
			 echo'<tr class="mtr">';
			            echo '  
			            	<td>  
			                        Vote_count:
                            </td>
			                <td>  
			                        '.$row['vcount'].'
                            </td>
                            
                            ';  

			echo'</tr>';
			 echo'<tr class="mtr">';
			            echo '  
			            	<td>  
			                        Phone_no:
                            </td>
			                <td>  
			                        '.$row1['CELL_NO'].'
                            </td>
                            
                            ';  

			echo'</tr>';
			echo'<tr class="mtr">';
			            echo '  
			            	<td>  
			                        Address:
                            </td>
			                <td>  
			                        '.$row1['ADDRESS'].'
                            </td>
                            
                            ';  

			echo'</tr>';
			echo'<tr class="mtr">';
			            echo '  
			            	<td>  
			                        E-MAIL:
                            </td>
			                <td>  
			                        '.$row1['LOGIN'].'
                            </td>
                            
                            ';  

			echo'</tr>';
			    

        ?>  

    </table> 
			


	</div>
	<div class="but"><button>E-mail</button></div>
	</section>
	<?php
	  include_once 'footer.php'
	?>