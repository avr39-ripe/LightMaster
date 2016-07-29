'use strict';

var appStatus;
var binStates;
//Websockets
var websocket;

function onOpen(evt) {
	console.log.bind(console)("CONNECTED");
	
	appStatus = new AppStatusClass();
	appStatus.init();
	
	binStates = new BinStatesClass();
	// setTimeout(function() { binStates.wsGetAllButtons(); }, 500);
	// setTimeout(function() { binStates.wsGetAllStates(); }, 850);
	setTimeout(function() { binStates.enableButtons(true); }, 500);
	setTimeout(function() { binStates.enableStates(true); }, 850);
}

function onMessage(evt) {
//	console.log.bind(console)("Message recv: " + evt.data);
	if(evt.data instanceof ArrayBuffer) {
    	var bin = new DataView(evt.data);
    	
    	var cmd = bin.getUint8(wsBinConst.wsCmd);
    	var sysId = bin.getUint8(wsBinConst.wsSysId);
    	var subCmd = bin.getUint8(wsBinConst.wsSubCmd);
    	console.log.bind(console)(`cmd = ${cmd}, sysId = ${sysId}, subCmd = ${subCmd}`);
    	
    	if ( cmd == wsBinConst.getResponse && sysId == 1 ) {
    		appStatus.wsBinProcess(bin);
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

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	initWS();
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);