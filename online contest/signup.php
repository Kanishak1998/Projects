<?php
  include_once 'header.php';
?>
<section class="main-container">
	<div class="main-wrapper">
		<p><br></p>
		<h2 style="text-align: center;font-size:30px;margin-left: -40px;">Sign-up</h2>
		<form class="signup-form" action="includes/signup.inc.php" method="POST">
			<input type="text" placeholder="Enter Fullname" name="name" required>
		    <input type="email" placeholder="Enter Email" name="uid" required>
		    <input type="password" placeholder="Enter Password" name="psw" required>
		    <input type="text" placeholder="Address" name="add" required>
		    <input type="number" placeholder="Conatct no" name="no" required>
		    <select name="role">
  <option value="voter">Voter</option>
  <option value="photographer">Photographer</option>
</select>

		    <button type="submit" name="submit">Sign up</button>

</div>
</section>
<?php
  include_once 'footer.php'
?>