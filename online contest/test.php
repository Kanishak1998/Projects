<?php

      include 'includes/dbh.inc.php';
      include 'vheader.php';
$n=$_GET["a"];
$query = "SELECT * FROM tbl_images WHERE id='$n'";  
        $result = mysqli_fetch_assoc(mysqli_query($conn, $query));  
        $l=$result["vcount"]+1;
        $query = "UPDATE tbl_images SET vcount ='$l' WHERE id='$n' ";
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
			<p class="ma"><b>Voter</b></p>
			<p class="head">Your Vote Has been Casted.CLick Vote More To  Cast More Vote For Other Photos</p>
			<button><a href="voterimg1.php">Vote More</a></button>



	</div>
	</section>
	<?php
	  include_once 'footer.php'
	?>