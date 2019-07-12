<?php  
include 'pheader.php';
include 'includes/dbh.inc.php'; 
 if(isset($_POST["insert"]))  
 {  
 	echo "$name ";
      $file = addslashes(file_get_contents($_FILES["image"]["tmp_name"]));  
      $query = "INSERT INTO tbl_images(name,pid) VALUES ('$file','$name')";  
      if(mysqli_query($conn, $query))  
      {  
           echo "Your's Image Inserted into Database";  
      }  
 }  
 ?>  
<html>
<body>
	<style >
		body
		{
			text-align: center;
		}
		form
		{
			font-size: 10px;
		}
	</style>

			<p class="man"><br></p>
<div class="tble">
                <h3 align="center" style="font-size:50px;">Insert your image</h3>  
                <br >  
                <form method="post" enctype="multipart/form-data">  
                     <input type="file" name="image" id="image" />  
                     <br >  <br>
                     <input type="submit" name="insert" id="insert" value="Insert" class="btn btn-info" />  
                </form>  
                <br >  
                
</div> 
       </body>
       </html>
 <script>  
 $(document).ready(function(){  
      $('#insert').click(function(){  
           var image_name = $('#image').val();  
           if(image_name == '')  
           {  
                alert("Please Select Image");  
                return false;  
           }  
           else  
           {  
                var extension = $('#image').val().split('.').pop().toLowerCase();  
                if(jQuery.inArray(extension, ['gif','png','jpg','jpeg']) == -1)  
                {  
                     alert('Invalid Image File');  
                     $('#image').val('');  
                     return false;  
                }  
           }  
      });  
 });  
 </script>  