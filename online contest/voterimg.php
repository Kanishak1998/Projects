<?php  
 $connect = mysqli_connect("localhost", "root", "", "photography");  
 ?>
<!DOCTYPE html>  
 <html>  
      <head>  
           <title>Webslesson Tutorial | Insert and Display Images From Mysql Database in PHP</title>  
      <body >  
           <br><br >  
           <div class="container" style="width:500px;">  
                <h3 align="center">view your image</h3>  
                <br >  
                <br >  
                <br >  
                <table class="table table-bordered">  
                     <tr>  
                          <th>Image</th>  
                     </tr>  
                <?php  
                $query = "SELECT * FROM tbl_images ORDER BY id DESC";  
                $result = mysqli_query($connect, $query);  
                echo'<tr>';
                while($row = mysqli_fetch_array($result))  
                {  
                     echo '  
                           
                               <td>  
                                    <img src="data:image/jpeg;base64,'.base64_encode($row['name'] ).'" height="200" width="200" class="img-thumnail" />  
                                    ';  
               
                              
                     

                }  
              echo'</tr>';
                ?>  
                </table>  
           </div>  
      </body>  
 </html>    