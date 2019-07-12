<?php
session_start();
$name=$_SESSION["uname"];
?>
<html>
<head>
  <title>Admin</title>
  <link rel="stylesheet" type="text/css" href="style.css">
</head>
<style>
a
{
  margin-left: 20px;
}
a:hover { 
  background-color: yellow;

}
.nav-login
{
  margin-top: 20px;
}
header .nav-display
{
  margin-top: 8px;
  float:right;
}

p.l
{
  style="font-family: arial;
  font-size: 30px;
  color: #111;
  line-height: 50px;
  margin-top: -50px;
  margin-left: 200px;
  margin-right: -80px;
}
</style>
<body>
<header> 
	<nav>
		<div class="main-wrapper">
			<ul>
				<li><a href="admin.php"> Home </a></li>
			</ul>
      <ul>
        <li><a href="aviewphotos.php">View Your Photos</a></li>
      </ul>
      <ul>
        <li><a href="alead.php">Check Leaderboard</a></li>
      </ul>
      <ul>
        <li><a href="winner.php">Declare Winner</a></li>
      </ul>
      
      

        <div class="nav-display">
           
          <p style="font-family: arial;
  font-size: 20px;
  color: #111;
  line-height: 50px;">Admin: <?php echo $name;?></p><p class="l"><a href="index.php" >Logout</a></p>
          
      </div>
	</nav>
</header>
</body>
</html>