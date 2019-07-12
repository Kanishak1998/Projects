<?php
session_start();
include 'dbh.inc.php';
if(isset($_POST['submit']))
{
	$id=$_POST['uid'];
	$pass=$_POST['pwd'];
	if(empty($id) || empty($pass))
	{
			header("Location: ../index.php?login=error1");
		exit();
	}
	else
	{
		$sql="SELECT * FROM user WHERE login='$id'";
		$result=mysqli_query($conn,$sql);
		$resultCheck=mysqli_num_rows($result);
		if ($resultCheck<1) 
		{

			header("Location: ../index.php?login=erroros");
			exit();
		}
		else
		{
			$row=mysqli_fetch_assoc($result);
			
			if($row['PASSWORD']!=$pass)
			{
				header("Location: ../index.php?login=incorr passward");
				exit();
			}
			else
			{
					$_SESSION["uid"]=$row["LOGIN"];
					$_SESSION["uname"]=$row["NAME"];
					$_SESSION["role"]=$row["ROLE_NAME"];
					if($row["ROLE_NAME"]=="Admin")
					{
							header("Location: ../admin.php");
					}
					else if($row["ROLE_NAME"]=="voter")
					{
						header("Location: ../voter.php");
					}
					else if($row["ROLE_NAME"]=="photographer")
					{
						header("Location: ../photographer.php");
					}
					
				exit();
			}
		
		}
	}
}
else
{
		header("Location: ../index.php?login=error");
		exit();
}
?>
