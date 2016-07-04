'use strict';

//wsBinProtocol constants
const wsBinConst = {
//Frame header offsets
	wsCmd			: 0, //Command type
	wsSysId			: 1, //target sysId
	wsSubCmd		: 2, //sub-command type
	wsPayLoadStart	: 3,

	reservedCmd		: 0,
	getCmd			: 1,
	setCmd			: 2,
	getResponse		: 3,
	setAck			: 4,
	setNack			: 5,

// sub-command
	scAppSetTime	: 1,
	scAppGetStatus	: 2
};

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
	if(evt.data instanceof ArrayBuffer) {
    	var bin = new DataView(evt.data);
    	
    	var cmd = bin.getUint8(wsBinConst.wsCmd);
    	var sysId = bin.getUint8(wsBinConst.wsSysId);
    	var subCmd = bin.getUint8(wsBinConst.wsSubCmd);
    	console.log.bind(console)(`cmd = ${cmd}, sysId = ${sysId}, subCmd = ${subCmd}`);
    	if ( cmd == wsBinConst.getResponse && subCmd == wsBinConst.scAppGetStatus) {
    		var counter = bin.getUint32(wsBinConst.wsPayLoadStart, true);
    		var timestamp = bin.getUint32(wsBinConst.wsPayLoadStart + 4, true);
    		console.log.bind(console)(`counter = ${counter}, timestamp = ${timestamp}`);
    	}
  	} else {
    	var json = JSON.parse(evt.data);
		console.log.bind(console)("Json recv: " + json);
	
		if (json.response == "getAppState") {
			onGetAppState(json);
		}
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
	websocket.binaryType = 'arraybuffer';
}

function closeWS() {
	websocket.close();
}

function wsGetAppStatus() {
//	event.preventDefault();
	var ab = new ArrayBuffer(3);
	var bin = new DataView(ab);
	var d = new Date();
	
	bin.setUint8(wsBinConst.wsCmd, wsBinConst.getCmd);
	bin.setUint8(wsBinConst.wsSysId, 1); //AppClass.sysId = 1
	bin.setUint8(wsBinConst.wsSubCmd, wsBinConst.scAppGetStatus);
	
	websocket.send(bin.buffer);
}

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	//Init
	initWS();
	// updateState();
	// setInterval(updateState, 5000);
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);