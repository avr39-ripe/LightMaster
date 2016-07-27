'use strict';

var binStates;
//Websockets
var websocket;

function onOpen(evt) {
	console.log.bind(console)("CONNECTED");
	
	wsGetAppStatus();
	setInterval(wsGetAppStatus, 5000);
	
	binStates = new BinStatesClass();
	setTimeout(function() { binStates.wsGetAllButtons(); }, 500)
	setTimeout(function() { binStates.wsGetAllStates(); }, 850)
}

function onMessage(evt) {
//	console.log.bind(console)("Message recv: " + evt.data);
	if(evt.data instanceof ArrayBuffer) {
    	var bin = new DataView(evt.data);
    	
    	var cmd = bin.getUint8(wsBinConst.wsCmd);
    	var sysId = bin.getUint8(wsBinConst.wsSysId);
    	var subCmd = bin.getUint8(wsBinConst.wsSubCmd);
    	console.log.bind(console)(`cmd = ${cmd}, sysId = ${sysId}, subCmd = ${subCmd}`);
    	
    	if ( cmd == wsBinConst.getResponse && sysId == 1 && subCmd == wsBinConst.scAppGetStatus ) {
    		var counter = bin.getUint32(wsBinConst.wsPayLoadStart, true);
    		var timestamp = bin.getUint32(wsBinConst.wsPayLoadStart + 4, true);
    		
    		document.getElementById("counter").textContent = counter;
    		var d = new Date();
    		d.setTime(timestamp * 1000);
    		document.getElementById("dateTime").textContent = d.toLocaleString();
    	}
    	
    	if ( cmd == wsBinConst.getResponse && ( sysId == 2 || sysId == 3) ) {
    		binStates.wsBinProcess(bin);
    	}
    		
  	} 
}

function onClose(evt) {
	console.log.bind(console)("DISCONNECTED");
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

function wsGetAppStatus() {
	var ab = new ArrayBuffer(3);
	var bin = new DataView(ab);
	
	bin.setUint8(wsBinConst.wsCmd, wsBinConst.getCmd);
	bin.setUint8(wsBinConst.wsSysId, 1); //AppClass.sysId = 1
	bin.setUint8(wsBinConst.wsSubCmd, wsBinConst.scAppGetStatus);
	
	websocket.send(bin.buffer);
}

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	initWS();
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);