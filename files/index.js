'use strict';

function BinStateClass (uid) {
	this.uid = uid;
	this._state = 0; //false
	this._name = "";
	this._initDone = false;
	
	this.wsGetName();
	this.wsGetState();
}

BinStateClass.prototype.wsGet = function (cmd) {
	var ab = new ArrayBuffer(3);
	var bin = new DataView(ab);
	
	bin.setUint8(wsBinConst.wsCmd, wsBinConst.getCmd);
	bin.setUint8(wsBinConst.wsSysId, 2); //BinStateHttpClass.sysId = 2
	bin.setUint8(wsBinConst.wsSubCmd, cmd);

	websocket.send(bin.buffer);
}

BinStateClass.prototype.wsGetName = function () {
	this.wsGet(1);
}

BinStateClass.prototype.wsGetState = function () {
	this.wsGet(2);
}

BinStateClass.prototype.wsGotName = function (bin) {
	var uid = bin.getUint8(wsBinConst.wsPayLoadStart, true);
	var strBuffer = new Uint8Array(bin.byteLength);
    for (var i = 0; i < strBuffer.length; i++) {
        strBuffer[i] = bin.getUint8(i);
    }
    this._name = new TextDecoder().decode(strBuffer)
    console.log.bind(console)(`uid = ${uid}, name = ${this._name}`);
    
    if ( !this._initDone ) {
    	this._initDone = true;
	   	var t = document.querySelector('#BinStateHttpClass');
	  
		var clone = document.importNode(t.content, true);
	  	clone.querySelector('#binState').textContent = `${this._name}`;
	  	clone.querySelector('#binStatePanel').id = `binStatePanel${this.uid}`;
		clone.querySelector('#binState').id = `binState${this.uid}`;
		
		var container = document.getElementById("panel-container");
		container.appendChild(clone);	
    }	
}

BinStateClass.prototype.wsGotState = function (bin) {
	var uid = bin.getUint8(wsBinConst.wsPayLoadStart, true);
	this._state = bin.getUint8(wsBinConst.wsPayLoadStart + 1, true);
    console.log.bind(console)(`name = {this._name}, uid = ${uid}, state = ${this._state}`);
    
    if ( this._initDone ) {
    	
    	var panel = document.querySelector(`#binStatePanel${this.uid}`);
    	
    	if (this._state) {
    		panel.classList.remove("panel-primary");
    		panel.classList.add("panel-danger");	
    	} else {
    		panel.classList.remove("panel-danger");
    		panel.classList.add("panel-primary");
    	}
    	
    }	
}

BinStateClass.prototype.wsBinProcess = function (bin) {
	var subCmd = bin.getUint8(wsBinConst.wsSubCmd);
	
	if ( subCmd == wsBinConst.scBinStateGetName) {
		this.wsGotName(bin);
	}
	
	if ( subCmd == wsBinConst.scBinStateGetState) {
		this.wsGotState(bin);
	}
	
}
	
// function wsGetAppState() {
	// var json = {};
	// json["command"] = "getAppState";
	// websocket.send(JSON.stringify(json));
// }

// function onGetAppState(json)
// {
	// Object.keys(json).forEach(function(key) {
		// if(key != "response") {
				// document.getElementById(key).innerHTML = json[key];
		// }
	// });
// }

//Websockets
var websocket;
function onOpen(evt) {
	console.log.bind(console)("CONNECTED");
	wsGetAppStatus();
	setInterval(wsGetAppStatus, 5000);
//	websocket.send("Sming love WebSockets");
	binState = new BinStateClass(0);
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
    	
    	if ( cmd == wsBinConst.getResponse && sysId == 1 && subCmd == wsBinConst.scAppGetStatus ) {
    		var counter = bin.getUint32(wsBinConst.wsPayLoadStart, true);
    		var timestamp = bin.getUint32(wsBinConst.wsPayLoadStart + 4, true);
    		console.log.bind(console)(`counter = ${counter}, timestamp = ${timestamp}`);
    		
    		document.getElementById("counter").textContent = counter;
    		var d = new Date();
    		d.setTime(timestamp * 1000);
    		document.getElementById("dateTime").textContent = d.toLocaleString();
    	}
    	
    	if ( cmd == wsBinConst.getResponse && sysId == 2 ) {
    		binState.wsBinProcess(bin);
    	}
    	// if ( cmd == wsBinConst.getResponse && sysId == 2 && subCmd == wsBinConst.scBinStateGetName ) {
    		// var uid = bin.getUint8(wsBinConst.wsPayLoadStart, true);
    		// var strBuffer = new Uint8Array(bin.byteLength);
            // for (var i = 0; i < strBuffer.length; i++) {
                // strBuffer[i] = bin.getUint8(i);
            // }
            // var name = new TextDecoder().decode(strBuffer)
            // console.log.bind(console)(`uid = ${uid}, name = ${name}`);
    	// }
//     	
		// if ( cmd == wsBinConst.getResponse && sysId == 2 && subCmd == wsBinConst.scBinStateGetState ) {
    		// var uid = bin.getUint8(wsBinConst.wsPayLoadStart, true);
    		// var state = bin.getUint8(wsBinConst.wsPayLoadStart + 1, true);
            // console.log.bind(console)(`uid = ${uid}, state = ${state}`);
    	// }
    	
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
	
	bin.setUint8(wsBinConst.wsCmd, wsBinConst.getCmd);
	bin.setUint8(wsBinConst.wsSysId, 1); //AppClass.sysId = 1
	bin.setUint8(wsBinConst.wsSubCmd, wsBinConst.scAppGetStatus);
	
	websocket.send(bin.buffer);
}

function wsBinStateGet(cmd) {
//	event.preventDefault();
	var ab = new ArrayBuffer(3);
	var bin = new DataView(ab);
	
	bin.setUint8(wsBinConst.wsCmd, wsBinConst.getCmd);
	bin.setUint8(wsBinConst.wsSysId, 2); //AppClass.sysId = 1
	bin.setUint8(wsBinConst.wsSubCmd, cmd);
	
	websocket.send(bin.buffer);
}

//Here we put some initial code which starts after DOM loaded
var binState; 
function onDocumentRedy() {
	//Init
	initWS();
	// updateState();
	// setInterval(updateState, 5000);
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);