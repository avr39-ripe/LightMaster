'use strict';

function getJson(url) {
	return fetch(url).then(function(response){if (response.status >= 200 && response.status < 300) return response.json();});
};

function post_button(key,state) {
	var json = {};
	json["state"] = state;
	
	// fetch(`/button?button=${key}`, {
		// method: 'post',
		// headers: {
			// 'Accept': 'application/json',
			// 'Content-Type': 'application/json; charset=utf-8'
			// },
		// body: JSON.stringify(json)
	// });
	json["command"] = "setButton";
	json["button"] = key;
	websocket.send(JSON.stringify(json));
	// getJson(`/button?button=${key}`)
	// .then(function(Json){updateButton(key,Json.state);});
}

function updateButton(key,state) {
	var button = document.getElementById(`Button${key}`);
	if (state == 1) {
		button.classList.remove("btn-primary");
		button.classList.add("btn-warning");
	} else if (state == 0) {
		button.classList.remove("btn-warning");
		button.classList.add("btn-primary");
	}
}

function initButtons() {
	getJson("/button")
	.then(function(Json){Object.keys(Json).forEach(function(key){
		var container = document.getElementById("panel-container");
		container.insertAdjacentHTML("beforeEnd",`<div class="col-xs-10 col-md-5"><br><button id="Button${key}" class="btn btn-primary btn-block">${Json[key].name}</button></div>`);
		var button = document.getElementById(`Button${key}`);
		button.addEventListener('mousedown', function() { post_button(key, 1); });
		button.addEventListener('mouseup', function() { post_button(key, 0); });
		updateButton(key,Json[key].state);
	})})
}

function onGetButtons(Json) {
	Object.keys(Json).forEach(function(key){
		if(key != "response") {
			var container = document.getElementById("panel-container");
			container.insertAdjacentHTML("beforeEnd",`<div class="col-xs-10 col-md-5"><br><button id="Button${key}" class="btn btn-primary btn-block">${Json[key].name}</button></div>`);
			var button = document.getElementById(`Button${key}`);
			button.addEventListener('mousedown', function() { post_button(key, 1); });
			button.addEventListener('mouseup', function() { post_button(key, 0); });
			updateButton(key,Json[key].state);	
		}
	})
}

function updateButtons() {
	getJson("/button")
	.then(function(Json){Object.keys(Json).forEach(function(key){
		updateButton(key,Json[key].state);
	})})
}

function onOpen(evt) {
	console.log.bind(console)("CONNECTED");
	wsGetButtons();
//	websocket.send("Sming love WebSockets");
}

function onClose(evt) {
	console.log.bind(console)("DISCONNECTED");
}

function onMessage(evt) {
	console.log.bind(console)("Message recv: " + evt.data);
	var json = JSON.parse(evt.data);
	console.log.bind(console)("Json recv: " + json);
	
	if (json.response == "getButton") {
		updateButton(json.button,json.state);
	}
	
	if (json.response == "getButtons") {
		onGetButtons(json);
	}
	//websocket.close();
}

function onError(evt) {
	console.log.bind(console)("ERROR: " + evt.data);
}

function initWS() {
	var wsUri = "ws://" + location.host + "/";
	websocket = new WebSocket(wsUri);
	websocket.onopen = function(evt) { onOpen(evt) };
	websocket.onclose = function(evt) { onClose(evt) };
	websocket.onmessage = function(evt) { onMessage(evt) };
	websocket.onerror = function(evt) { onError(evt) };
}

function closeWS() {
	websocket.close();
}

function wsGetButtons() {
	var json = {};
	json["command"] = "getButtons";
	websocket.send(JSON.stringify(json));
}

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	//Init
	initWS();
	
//	initButtons();
//	updateButtons();
//	setInterval(updateButtons, 3000);

}

var websocket;

// var wsUri = "ws://" + location.host + "/";
// var websocket = new WebSocket(wsUri);
// websocket.onopen = function(evt) { onOpen(evt) };
// websocket.onclose = function(evt) { onClose(evt) };
// websocket.onmessage = function(evt) { onMessage(evt) };
// websocket.onerror = function(evt) { onError(evt) };

//window.addEventListener('focus', initWS);
//window.addEventListener('blur', closeWS);
document.addEventListener('DOMContentLoaded', onDocumentRedy);