'use strict';

const statusFlags = { INVALID: 1, DISCONNECTED: 2};

function updateState() {
    var xhr1 = new XMLHttpRequest();
    
    xhr1.open('GET', '/state.json', true);

    xhr1.send();

    xhr1.onreadystatechange = function() {
        
        if (this.readyState != 4) return;
        if (this.status == 200) {
            if (this.responseText.length > 0) {
                var stateJson = JSON.parse(this.responseText);
                document.getElementById('counter').textContent = stateJson.counter;
            }
        }
    };
}

function wsGetAppState() {
	var json = {};
	json["command"] = "getAppState";
	websocket.send(JSON.stringify(json));
}

function onGetAppState(json)
{
	Object.keys(json).forEach(function(key) {
		if(key != "response") {
				document.getElementById(key).innerHTML = json[key];
		}
	});
}

//Websockets
var websocket;
function onOpen(evt) {
	console.log.bind(console)("CONNECTED");
	wsGetAppState();
	setInterval(wsGetAppState, 5000);
//	websocket.send("Sming love WebSockets");
}

function onClose(evt) {
	console.log.bind(console)("DISCONNECTED");
}

function onMessage(evt) {
	console.log.bind(console)("Message recv: " + evt.data);
	var json = JSON.parse(evt.data);
	console.log.bind(console)("Json recv: " + json);
	
	if (json.response == "getAppState") {
		onGetAppState(json);
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

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	//Init
	initWS();
	// updateState();
	// setInterval(updateState, 5000);
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);