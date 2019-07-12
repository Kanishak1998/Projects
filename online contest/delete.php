<?php

      include 'includes/dbh.inc.php';
      include 'aheader.php';
$n=$_GET["a"]; 
$query = "DELETE FROM tbl_images WHERE id='$n' ";
mysqli_query($conn, $query); 
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
</style>
	<section>
		<div >
			<p class="man"><br></p>
			<p class="ma"><b>Admin</b></p>
			<p class="head">Photo Has been Deleted</p>
			<button><a href="aviewphotos.php">Check Others</a></button>



	</div>
	</section>
	<?php
	  include_once 'footer.php'
	?>