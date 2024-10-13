<?php

// connect to database
$server = "webdb.uvm.edu";
$username = "emmalloy_writer";
$password = "JFVJ5YlWonCK";
$databaseName = "EMMALLOY_labs";
$db = mysqli_connect($server, $username, $password, $databaseName);

if ($db) {
	$gameId = $_GET["gameId"];
	if (is_numeric($gameId)) {
		// on game page for game with gameId
		$sql = "SELECT player1ToMove, player1 FROM Games WHERE gameId=$gameId";
		$result = mysqli_query($db, $sql);
		// if the game exists
		if ($result && mysqli_num_rows($result) > 0) {
			$game = mysqli_fetch_assoc($result);
			// if it's the user's move, refresh the page (poll only runs if it's not the user's turn on the client)
			$userIsPlayer1 = $_GET["username"] === $game["player1"];
			$userToMove = intval($game["player1ToMove"]) == $userIsPlayer1;
			if ($userToMove) {
				echo "refresh";
			}
		}
	} else {
		// on challenge page
		$username = $_GET["username"];
		$clientChallenge = $_GET["incomingChallenge"];
		$sql = "SELECT incomingChallenge FROM Users WHERE username='$username'";
		$result = mysqli_query($db, $sql);
		if ($result && mysqli_num_rows($result) > 0) {
			$user = mysqli_fetch_assoc($result);
			$serverChallenge = $user["incomingChallenge"];
			if ($serverChallenge != $clientChallenge) {
				echo $serverChallenge;
			}
		}
	}		
		
	mysqli_close($db);
}

?>
