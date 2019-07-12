<?php
include 'dbh.inc.php';
if(isset($_POST['submit']))
{
	//include_once 'dbh.inc.php';
	/*$name=mysqli_real_escape_string($conn,$_POST['name']);
	$id=mysqli_real_escape_string($conn,$_POST['uid']);
	$pass=mysqli_real_escape_string($conn,$_POST['psw']);
	$add=mysqli_real_escape_string($conn,$_POST['add']);
	$num=mysqli_real_escape_string($conn,$_POST['no']);
	$gid=mysqli_real_escape_string($conn,$_POST['role']);*/
	$name=$_POST['name'];
	$id=$_POST['uid'];
	$pass=$_POST['psw'];
	$add=$_POST['add'];
	$num=$_POST['no'];
	$gid=$_POST['role'];


	//error handlers
	//check empty fields
	if(empty($name) || empty($id) || empty($pass) || empty($add) || empty($num) ||empty($gid))
	{
		header("Location: ../signup.php?signup=empty");
		exit();
	}
	else
	{
		if(!preg_match("/^[a-zA-Z]*$/",$name))
		{
			header("Location: ../signup.php?signup=invalid name");
			exit();
		}
		else
		{
			if(!filter_var($id,FILTER_VALIDATE_EMAIL))
			{
				header("Location: ../signup.php?signup=invalid email");
			exit();
			}
			else
			{
				$sql="SELECT *FROM user WHERE login='$id'";
				$result=mysqli_query($conn,$sql);
				$resultCheck=mysqli_num_rows($result);
				if ($resultCheck>0) {

					header("Location: ../signup.php?signup=email already exists");
					exit();
				}
				else
				{
					$sql="INSERT INTO user (NAME,LOGIN,PASSWORD,CELL_NO,ADDRESS,ROLE_NAME) VALUES ('$name','$id','$pass','$num','$add','$gid');";
					if(mysqli_query($conn,$sql))
					{
						header("Location: ../signup.php?signup=Success");
					}
					else
					{
						echo "eooe";
					}
					exit();

				}

			}
		}
	}
}
else
{
	header("Location: ../signup.php");
	exit();
}
?>