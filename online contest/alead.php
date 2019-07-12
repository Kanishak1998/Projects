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
p.head
{
  font-family: arial;
  font-size: 20px;
  color: #111;
  line-height: 50px;  
  background-color: lightblue;

}
	table, th, td {
    border: 1px solid black;
    font-family: arial;
  font-size: 20px;
  color: #111;
  line-height: 50px;  
  background-color: orange;
  width: 50%;
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
	margin-left: 450px;
}
th {
  height: 50px;
}
}
</style>
	<section>
			<p class="man"><br></p>
			<p class="ma"><b>Admin</b></p>
			<p class="ma"> Learderboard</p><br>

		<div class="tble">
			<table class="main">  
				<tr >
					<th>Rank</th>
					<th>Phtographer Name</th>
					<th>Photo_id</th>
					<th>Vote count</th>
				</tr>
        <?php  
        $query = "SELECT * FROM tbl_images ORDER BY vcount DESC";  
        $result = mysqli_query($conn, $query);  
        $rank=1;
        while($row = mysqli_fetch_array($result))  
        {  
        	
			echo'<tr class="mtr">';
			            echo '  
			            	<td>  
			                        '.$rank.'
                            </td>
			                <td>  
			                        '.$row['pid'].'
                            </td>
                            <td>  
			                        '.$row['id'].'
                            </td>
                            <td>  
			                        '.$row['vcount'].'
                            </td>
                            ';  

			echo'</tr>';
			$rank=$rank+1;
			    
        }  
        ?>  
    </table>
			
	</div>
	</section>
	<?php
	  include_once 'footer.php'
	?>