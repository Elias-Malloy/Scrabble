// check for updates from the server
// front load all this logic so it isn't run on every poll
// these things only change on a page refresh;
// get elements
const form = document.getElementById("userIn");
const usernameE = document.getElementById("username");
const currentGameE = document.querySelector('input[name="currentGame"]:checked');
const incomingChallengeE = document.getElementById("incomingChallenge");
var incomingChallengeP;
// variables for GET url
// (I'm pretending javascript has sensible variable scoping rules for my own sanity)
var username;
var gameId;
var toMove;
var incomingChallenge;
var url;

if (usernameE != null && currentGameE != null) {
	username = usernameE.value;
	gameId = currentGameE.value;
	if (username != "") {
		if (incomingChallengeE != null) {
			incomingChallengeP = incomingChallengeE.firstChild;
			if (incomingChallengeP != null) {
				incomingChallenge = incomingChallengeP.value;
				url = "poll.php?username=" + username + "&incomingChallenge=" + incomingChallenge;
				// poll for new challenge every second
				setInterval(poll, 1000);
			}
		} else {
			url = "poll.php?username=" + username + "&gameId=" + gameId;
			// poll for opponent's move every second
			setInterval(poll, 1000);
		}
	}
}

// call poll.php
function poll() {
    // function partly from stack overflow, cited in README.md
    var xmlhttp;
    xmlhttp = new XMLHttpRequest();
    xmlhttp.onreadystatechange = function() {
        if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			if (xmlhttp.responseText == "") {
				// do nothing
			} else if (xmlhttp.responseText == "refresh") {
    			// opponent played
				form.submit();
			} else {
				// new challenge from another user
				// update without refreshing the page (annoying)
				if (incomingChallengeE.classList.contains("hidden")) {
					incomingChallengeE.classList.remove("hidden");
				}
				incomingChallengeP.innerHTML = xmlhttp.responseText;
				// updating polling url to reflect changed challenge
				incomingChallenge = xmlhttp.responseText;
				url = "poll.php?username=" + username + "&incomingChallenge=" + incomingChallenge;
			}
		}
    }
   
    // call poll.php with get string
	xmlhttp.open("GET", url, true);
    xmlhttp.send();
}
