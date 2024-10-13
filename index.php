<!DOCTYPE html>
<html>
<head>
<title>Scrabble</title>
<link rel="stylesheet" href="style.css">
<link rel="icon" type="image/png" href="letters/S.png">
</head>

<body>
<form id="userIn" name="userIn" method="post">
<input type="text" id="command" name="command" class="hidden">
<input type="text" id="word" name="word" class="hidden">
<div class="" id="board">
<img id="backdrop" src="ScrabbleBoard.png"/>
<?php 
	// compile c program for processing turn (temporary)
	$output = "";
	exec("gcc processTurn.c -o processTurn.exe", $output);
	// connect to database
	$server = "webdb.uvm.edu";
	$username = "emmalloy_writer";
	$password = "JFVJ5YlWonCK";
	$databaseName = "EMMALLOY_labs";
	$db = mysqli_connect($server, $username, $password, $databaseName);
	
	if (!$db) {
		echo "connection to db failed: " . mysqli_connect_error();
	}
	
	$command = $_POST["command"];
	$username = $_POST["username"];
	$password = $_POST["password"];
	$currentGame = $_POST["currentGame"];
	$formMessage = "";

	$loggedIn = false;
	$incomingChallenge = "";
	$games = array();
	$selected = null;

	if ($username && $password) {
		$passwordHash = password_hash($password, PASSWORD_DEFAULT);
		$sql = "SELECT * FROM Users WHERE username='$username'";
	//	$query = mysqli_prepare($db, $sql);
	//	if (!$query) {
	//		die("query error");
	//	}
		$result = mysqli_query($db, $sql);
		if (mysqli_num_rows($result) > 0) {
			$row = mysqli_fetch_assoc($result);
			if (password_verify($password, $row["passwordHash"])) {
				$incomingChallenge = $row["incomingChallenge"];
				$loggedIn = true;
			} else {
				$formMessage = "Incorrect password";
			}
		} else {
			// register user
			$sql = "INSERT INTO Users (username, passwordHash) VALUES ('$username', '$passwordHash')";
			$result = mysqli_query($db, $sql);
			if (!$result) {
				$loggedIn = false;
				$formMessage = "Registration failed";	
			} else {
				$loggedIn = true;
			}

		}
	}

	// if the user presses the log out button, log them out
	if ($command === "logout") {
		$loggedIn = false;
	}

	// if the user submitted a challenge, send the challenge
	$challenged = $_POST["challenged"]; 
	if ($loggedIn && $challenged && $challenged !== $username) {
		$sql = "SELECT * FROM Users WHERE username='$challenged'";
		$result = mysqli_query($db, $sql);
		if ($result && mysqli_num_rows($result) > 0) {
			$sql = "UPDATE Users SET incomingChallenge='$username' WHERE username='$challenged'";
			mysqli_query($db, $sql);
			$formMessage = "Challenge sent!";
		} else {
			$formMessage = "$challenged is not registered";
		}	
	}

	if ($loggedIn && $incomingChallenge && $command === "accept-challenge") {
		$sql = "SELECT * FROM Users WHERE username='$incomingChallenge'";
		$result = mysqli_query($db, $sql);
		if (mysqli_num_rows($result) > 0) {
			// create new game with the user who submitted the challenge
			$returnCode = 0;
			$output = "";
			$args = "create-new";
			exec("./processTurn.exe $args", $output, $returnCode);
			$sql = "INSERT INTO Games (board, bag, player1, player2, hand1, hand2,";
			$sql .= "score1, score2, player1ToMove) VALUES ('$output[0]', ";
			$sql .= "'$output[1]', '$username', '$incomingChallenge', '$output[2]', ";
			$sql .= "'$output[3]', '$output[4]', '$output[5]', $output[6])";
			mysqli_query($db, $sql);
			
			// remove the accepted challenge from user 
			$sql = "UPDATE Users SET incomingChallenge=null WHERE username='$username'";	
			mysqli_query($db, $sql);
		}
		$incomingChallenge = "";
	}

	if ($loggedIn && $incomingChallenge && $command === "reject-challenge") {
		$sql = "UPDATE Users SET incomingChallenge=null WHERE username='$username'";	
		mysqli_query($db, $sql);
		$incomingChallenge = "";
	}

	// if logged in, display the user's games and assign currently viewed game to $selected
	if ($loggedIn) {
		$sql = "SELECT * FROM Games WHERE player1='$username' OR player2='$username'";
		$result = mysqli_query($db, $sql);
		$numGames = 0;
		if ($result) {
			$numGames = mysqli_num_rows($result); 
		}
		// fill games array
		for ($i = 0; $i < $numGames; $i++) {
			$row = mysqli_fetch_assoc($result);
			$games[] = $row;
		}
		echo '<div id="games-tab">';
		if ($numGames > 0) {
			echo '<h3>Games</h3>', "\n";
		}
		// fill out games tab with the games the user is in
		foreach ($games as $game) {
			$strGameId = (string) $game["gameId"];
			$opponent = ($username === $game["player1"]) ? $game["player2"] : $game["player1"];
			echo '<label onclick="changeGame()">';
			echo '<input type="radio" name="currentGame" value="'.$strGameId.'" ';
			if ($strGameId === $currentGame) {
				echo 'checked';
				$selected = $game;
			}
			echo '>';
			echo 'vs '.$opponent.'</label>';
		}
		echo '<input type="radio" name="currentGame" id="new-game" value="challenge" ';
		if (!$selected) {
			echo 'checked';
		}
		echo '>';
		echo '</div>'; // end games-tab
		
		// save username and password on page in hidden fields
		echo '<input class="hidden" type="text" id="username" name="username" value="'.$username.'">', "\n";
		echo '<input class="hidden" type="password" id="password" name="password" value="'.$password.'">', "\n";
		// display log out button
		echo '<img class="tile button" id="253" src="icons/NewGame.png" onclick="newGameTab()" />';
		echo '<img class="tile button" id="254" src="icons/SignOut.png" onclick="logout()" />';
	}

// major phase
	if (!$loggedIn) {
		// output login form
		echo '<div id="form-tile" class="login">', "\n";
        echo '<h1>Login</h1>', "\n";
    //    echo '<label for="username">username</label>', "\n";
        echo '<img src="icons/UserName_Blue.png" />';
		echo '<input type="text" id="username" name="username" placeholder="username">', "\n";
    //     echo '<label for="password">password</label>', "\n";
        echo '<img src="icons/Password_Blue.png" />';
        echo '<input type="password" id="password" name="password" placeholder="password">', "\n";
        echo '<input type="submit">', "\n";
		if ($formMessage) {
			echo '<p>'.$formMessage.'</p>';
		}
        echo '</div>', "\n";
	} else if(!$selected) {
		// output challenge form
		echo '<div id="form-tile" class="login">', "\n";
        echo '<h1>New Game</h1>', "\n";
        echo '<label for="challenged">Challenge user</label>', "\n";
        echo '<input type="text" id="challenged" name="challenged">', "\n";
		echo '<input type="submit">', "\n";
		
		if ($formMessage) {
			echo '<p>'.$formMessage.'</p>';
		}
		
		$hidden = "";
		if (!$incomingChallenge) {
			$hidden = "hidden";
		}
		
		echo '<div id="incomingChallenge" class="'.$hidden.'">';
		echo "<p>$incomingChallenge</p><p> challenged you to a game!</p>\n";
		echo '<img class="button" src="icons/RedX.png" onclick="rejectChallenge()" />';
		echo '<img class="button" src="icons/GreenCheck.png" onclick="acceptChallenge()" />';
		echo '</div>';
		
        echo '</div>', "\n";
	} else {
		// game is selected
		$gameId = $selected["gameId"];
		$board = $selected["board"];
		$hand = "";
		$opponent = "";
		$userToMove = false;
		if ($selected["player1"] === $username) {
			$hand = $selected["hand1"];
			$opponent = $selected["player2"];
		} else {
			$hand = $selected["hand2"];
			$opponent = $selected["player1"];
		}
		$score1 = $selected["score1"];
		$score2 = $selected["score2"];
		$player1ToMove = $selected["player1ToMove"];
		$winner = $selected["winner"];
		if (intval($player1ToMove)) {
			$userToMove = $username === $selected["player1"];
		} else {
			$userToMove = $username === $selected["player2"];
		}
		if ($command === "play-word" && $userToMove) {
			$output = "";
			$returnCode = 0;
			$args = '"' . $selected["board"] . '" "' . $selected["bag"] . '"';
			$args .= ' "' . $selected["hand1"] . '" "' . $selected["hand2"] . '"';
			$args .= ' "' . $selected["score1"] . '" "' . $selected["score2"] . '"';
			$args .= ' "' . $selected["player1ToMove"] . '" "' . $_POST["word"] . '"';
			exec('./processTurn.exe ' .$args, $output, $returnCode);
			if ($returnCode == 0) {
				// display tiles
				$board = $output[0];
				$bag = $output[1];
				$hand1 = $output[2];
				$hand2 = $output[3];
				$score1 = (int) $output[4];
				$score2 = (int) $output[5];
				$player1ToMove = $output[6];
				$winner = $output[7];
				// update database entry for game
				$sql = "UPDATE Games SET board='$board', bag='$bag', hand1='$hand1', hand2='$hand2', ";
				$sql .= "score1=$score1, score2=$score2, player1ToMove='$player1ToMove', winner='$winner' ";
				$sql .= "WHERE gameId=$gameId";
				mysqli_query($db, $sql);
			} else {
				$errorMessage = $output[0];
				if ($errorMessage) {
					echo '<p id="error-message">'.$errorMessage.'</p>', "\n";
				}
			}
		} else if ($command === "turn-in" && $userToMove) {

		} else if ($command === "resign") {
			$winner = (string) ($username === $selected["player2"]);
			echo "winner: '$winner'\n";
			$sql = "UPDATE Games SET winner='$winner' WHERE gameId=$gameId";
			mysqli_query($db, $sql);
			echo "updated!";
		}
		// output tiles placed on board

		for ($i = 0; $i < 225; $i++) {
			if ($board[$i] != '.') {
				echo '<img class="tile" id='.$i.' src="letters/'.$board[$i];
				if (!ctype_upper($board[$i])) {
					echo '_blank';
				}
				echo '.png"/>',"\n";
			}
		}
		// output deck
		echo '<div id="deck"></div>';


		// output tiles in hand
		$i = 232;
		for ($i = 0; $i < strlen($hand); $i++) {
			$squareId = $i + 229;
			echo '<img class="tile hand" id='.$squareId.' src="letters/'.$hand[$i];
			if (!ctype_upper($hand[$i])) {
				echo '_blank';
			}		
			echo '.png" />', "\n";
		}
		$userToMove = intval($player1ToMove) == ($username === $selected["player1"]);
		// only enable polling if it is not the user's move
		

		// output each player's score and name
		echo '<div id="player1Score" class="score-box">';
		echo '<p>'.$selected["player1"].'</p>';
		echo '<p>'.$score1.'</p>';
		if (intval($player1ToMove)) {
//			echo '<img src="icons/GreenCircle.png" style="width: 5%;" />';
		}
		echo '</div>';
		
		echo '<div id="player2Score" class="score-box">';
		echo '<p>'.$selected["player2"].'</p>';
		echo '<p>'.$score2.'</p>';
		if (!intval($player1ToMove)) {
//			echo '<img src="icons/GreenCircle.png" style="width: 5%;" />';
		}
		echo '</div>';
		if (!$winner) {
			echo '<img class="tile button" id="245" src="icons/Resign.png" onclick="resign()" />';
		}
		// output active game buttons (play word, recall, resign, turn in, shuffle)
		if ($userToMove) {
			echo '<img class="tile button" id="246" src="icons/TurnIn.png" onclick="turnIn()" />';
			echo '<img class="tile button" id="247" src="icons/Play.png" onclick="playWord()" />';
		} else {
			echo '<img class="tile" id="246" src="icons/TurnInBW.png" />';
			echo '<img class="tile" id="247" src="icons/PlayBW.png" />';
		}
		echo '<img class="tile button" id="248" src="icons/Shuffle.png" onclick="shuffle()" />';
		echo '<img class="tile button" id="249" src="icons/Recall.png" onclick="recall()" />';
	} 

	mysqli_close($db);
	
?>	
</div>
</form>
<?php

if (!$selected || !$userToMove) {
	echo '<script type="text/javascript" src="poll.js"></script>', "\n";
}

?>
<script type="text/javascript" src="scrabble.js"></script>
</body>
</html>
