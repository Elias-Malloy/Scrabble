const boardImg = document.getElementById("backdrop");
var selected = -1;

document.addEventListener("keydown", updateSelectedBlank);

function loadTiles() {
	console.log("loading...");
	const board = boardImg.getBoundingClientRect();
	var tiles = document.querySelectorAll(".tile");
	for (var i = 0; i < tiles.length; i++) {
		const tile = tiles[i];
		tile.addEventListener("click", (e) => {
			if (tile.classList.contains("hand")) {
				if (selected == -1) {
					selected = parseInt(tile.id);
					tile.classList.add("selected");
				} else {
					const selectedTile = document.getElementById(selected.toString());
					selectedTile.classList.remove("selected");
					selectedTile.id = tile.id;
					tile.id = selected.toString();
					selected = -1;
					updateTiles();
				}
			}
		});
	}
		
	var errorMessage = document.getElementById("error-message");
	if (errorMessage != null) {
		setTimeout(() => { errorMessage.classList.add("hidden"); }, 5000);
	}

	const deck = document.getElementById("deck");
	if (deck != null) {
		deck.addEventListener("click", moveSelectedToPressed);
	}
	updateTiles();
	console.log("loaded");
}

function updateSelectedBlank(e) {
    e = e || window.event;
	if (selected == -1)
		return;

	var tile = document.getElementById(selected.toString());
	if (tile == null)
		return;

	if (tile.src.substring(tile.src.length - 9, tile.src.length - 4) != "blank")
		return;

	// if key is alphabetical
	if (e.key.length == 1 && ((e.key >= "a" && e.key <= "z") || (e.key >= "A" && e.key <= "Z"))) {
		tile.src = "letters/" + e.key + "_blank.png";
	} else if (e.key == "Backspace" || e.key == "Delete") {
		tile.src = "letters/ _blank.png";
	}
}

function Submit() {
	const form = document.getElementById("userIn");
	form.submit();
}
function newGameTab() {
	var newGameRadio = document.getElementById("new-game");
	newGameRadio.checked = true;
	Submit();
}

function recall() {
	var hand = document.querySelectorAll(".hand");
	const deckStartId = 15 * 15 + 7 - Math.floor(hand.length / 2);

	for (var i = 0; i < hand.length; i++) {
		if (parseInt(hand[i].id) == selected) {
			hand[i].classList.remove("selected");
			selected = -1;
		}
		hand[i].id = (deckStartId + i).toString();
	}
	updateTiles();
}

function shuffle() {
	var hand = document.querySelectorAll(".hand");
	var deck = [];
	for (var i = 0; i < hand.length; i++) {
		const squareId = parseInt(hand[i].id);
		if (squareId >= 229 && squareId <= 235) { // deck range
			deck.push(hand[i]);
		}
	}
	for (var i = 0; i < deck.length * 20; i++) {
		const r1 = Math.floor(Math.random() * deck.length);
		const r2 = Math.floor(Math.random() * deck.length);
		const temp = deck[r1].id;
		deck[r1].id = deck[r2].id;
		deck[r2].id = temp;
	}
	updateTiles();
}

function playWord() {
	var hand = document.querySelectorAll(".hand");
	var word = "";    
	//https://emmalloy.w3.uvm.edu/Scrabble/letters/_.png
	//                                             ^-45

	for (var i = 0; i < hand.length; i++) {
		const squareId = parseInt(hand[i].id);
		if (squareId >= 0 && squareId < 225) { 	  
			const letter = hand[i].src.charAt(45); // this is a dumb way to do this
			word = word.concat(letter);
			word = word.concat(hand[i].id);
		}
	}
	console.log("playing word: " + word);
	var wordField = document.getElementById("word");
	wordField.value = word;
	var command = document.getElementById("command");
	command.value = "play-word";
	Submit();
}

function toggleTurnInDeck() {
	turnInDeck = document.getElementById("turnInDeck");
	if (turnInDeck != null) {
		
		recall()
	} else {

	}
}

function turnInTiles() {
	var hand = document.querySelectorAll(".hand");
	var word = "";

	for (var i = 0; i < hand.length; i++) {
		const squareId = parseInt(hand[i].id);
		if (squareId >= 259 && squareId <= 265) { // turn in deck range
			const letter = hand[i].src.charAt(45); // this is a dumb way to do this
			word = word.concat(letter);
		}
	}
	
	console.log("turning in: " + word + ".");
	var wordField = document.getElementById("word");
	wordField.value = word;
	var command = document.getElementById("command");
	command.value = "turn-in";
	Submit();
}

function logout() {
	var command = document.getElementById("command");
	command.value = "logout";
	Submit();
}

function acceptChallenge() {
	var command = document.getElementById("command");
	command.value = "accept-challenge";
	Submit();
}

function rejectChallenge() {
	var command = document.getElementById("command");
	command.value = "reject-challenge";
	Submit();
}

function resign() {
	var command = document.getElementById("command");
	command.value = "resign";
	Submit();
}


function changeGame() {
	Submit();
	loadTiles();
}

function updateTiles() {
	console.log("updating tiles...");
	const board = boardImg.getBoundingClientRect();
	var tiles = document.querySelectorAll(".tile");

	const squareWidth = (board.right - board.left) / 15;
	const tileWidth = (board.right - board.left) / 18;
	const tilePadding = (squareWidth - tileWidth) / 2;

	for (var i = 0; i < tiles.length; i++) {
		var squareId = parseInt(tiles[i].id);
		const x = squareId % 15;
		const y = Math.floor(squareId / 15);
		tiles[i].style.width = tileWidth.toString().concat("px");
		tiles[i].style.top = (tilePadding + squareWidth * y).toString().concat("px");
		tiles[i].style.left = (tilePadding + squareWidth * x).toString().concat("px");
	}

	console.log("positioning form-tile");
	var formTile = document.getElementById("form-tile");
	if (formTile != null) {
		formTile.style.width = (squareWidth * 5).toString().concat("px");
		formTile.style.height = (squareWidth * 7).toString().concat("px");
		formTile.style.top = (squareWidth * 4).toString().concat("px");
		formTile.style.left = (squareWidth * 5).toString().concat("px");
	}
	var deck = document.getElementById("deck");
	if (deck != null) {
		deck.style.width = (squareWidth * 7).toString().concat("px");
		deck.style.height = squareWidth.toString().concat("px");
		deck.style.top = (squareWidth * 15).toString().concat("px");
		deck.style.left = (squareWidth * 4).toString().concat("px");
	}
	var gamesTab = document.getElementById("games-tab");
	if (gamesTab != null) {
		gamesTab.style.width = (squareWidth * 2).toString().concat("px");
		gamesTab.style.top = (squareWidth * 16).toString().concat("px");
		gamesTab.style.left = (tilePadding).toString().concat("px");
	}

	var player1Score = document.getElementById("player1Score");
	if (player1Score != null) {
		player1Score.style.width = (squareWidth * 2).toString().concat("px");
		player1Score.style.height = squareWidth.toString().concat("px");
		player1Score.style.top = (squareWidth * 15).toString().concat("px");
		player1Score.style.left = (tilePadding).toString().concat("px");
	}
	var player2Score = document.getElementById("player2Score");
	if (player2Score != null) {
		player2Score.style.width = (squareWidth * 2).toString().concat("px");
		player2Score.style.height = squareWidth.toString().concat("px");
		player2Score.style.top = (squareWidth * 15).toString().concat("px");
		player2Score.style.left = (squareWidth * 13 - tilePadding).toString().concat("px");
	}
	var errorMessage = document.getElementById("error-message");
	if (errorMessage != null) {
		errorMessage.style.width = (squareWidth * 7).toString().concat("px");
		errorMessage.style.height = squareWidth.toString().concat("px");
		errorMessage.style.top = (squareWidth * 17).toString().concat("px");
		errorMessage.style.left = (squareWidth * 4).toString().concat("px");
	}
}

function moveSelectedToPressed(e) {
	const board = boardImg.getBoundingClientRect();
    const squareWidth = (board.right - board.left) / 15;
    const tileWidth = (board.right - board.left) / 18;
    const tilePadding = (squareWidth - tileWidth) / 2;
    const x = Math.floor((e.clientX + window.scrollX) / squareWidth);
    const y = Math.floor((e.clientY + window.scrollY) / squareWidth);
    const id = 15 * y + x;
    if (selected != -1) {
        var tile = document.getElementById(selected.toString());
        tile.id = id.toString();
        tile.classList.remove("selected");
        selected = -1;
        tile.style.width = tileWidth.toString().concat("px");
        tile.style.top = (tilePadding + squareWidth * y).toString().concat("px");
        tile.style.left = (tilePadding + squareWidth * x).toString().concat("px");
    }
}

window.addEventListener("load", loadTiles);
window.addEventListener("resize", updateTiles);

boardImg.addEventListener("click", moveSelectedToPressed);

