'use strict';

var appStatus;
var binStates;
//Websockets
var websocket;

function onOpen(evt) {
//	console.log.bind(console)("CONNECTED");
	
	appStatus = new AppStatusClass();
	appStatus.enable(true);
	
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
//    	console.log.bind(console)(`cmd = ${cmd}, sysId = ${sysId}, subCmd = ${subCmd}`);
    	
    	if ( cmd == wsBinConst.getResponse && sysId == 1 ) {
    		appStatus.wsBinProcess(bin);
    	}
    	
    	if ( cmd == wsBinConst.getResponse && ( sysId == 2 || sysId == 3) ) {
    		binStates.wsBinProcess(bin);
    	}
    		
  	} 
}

function onClose(evt) {
//	console.log.bind(console)("DISCONNECTED");
}

function onError(evt) {
//	console.log.bind(console)("ERROR: " + evt.data);
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

//DIRTY! FIXME
const statusFlags = { INVALID: 1, DISCONNECTED: 2};

function updateState() {
    var xhr = new XMLHttpRequest();
   
    xhr.open('GET', '/temperature.json', true);

    xhr.send();

    xhr.onreadystatechange = function() {
        
        if (this.readyState != 4) return;
        if (this.status == 200) {
            if (this.responseText.length > 0) {
                var tempJson = JSON.parse(this.responseText);
                Object.keys(tempJson).forEach(function(key) {
                    var panelDiv = document.getElementById('panel-temperature' + key);
                    panelDiv.className = '';
                    panelDiv.classList.add("panel");
                    if (tempJson[key].statusFlag == 0) {
                        panelDiv.classList.add("panel-default");
                    } else if (tempJson[key].statusFlag & statusFlags.DISCONNECTED) {
                        panelDiv.classList.add("panel-danger");
                    } else if (tempJson[key].statusFlag & statusFlags.INVALID) {
                        panelDiv.classList.add("panel-warning");
                    }
                    var bodyDiv = document.getElementById('body-temperature' + key);
                    bodyDiv.innerHTML = tempJson[key].temperature + ' &deg;C';
               });
            }
        }
   };
}

function initTemperature() {
	var xhr = new XMLHttpRequest();

	xhr.open('GET', '/temperature.json', true);

	xhr.send();


    xhr.onreadystatechange = function() {

        if (this.readyState != 4)
            return;
        if (this.status == 200) {
            if (this.responseText.length > 0) {
                var tempJson = JSON.parse(this.responseText);
                Object.keys(tempJson).forEach(function(key) {
                    var colDiv = document.createElement('div');
                    colDiv.classList.add("col-xs-10");
                    colDiv.classList.add("col-md-5");
                    var panelDiv = document.createElement('div');
                    panelDiv.classList.add("panel");
                    panelDiv.id = "panel-temperature" + key;
                    if (tempJson[key].statusFlag == 0) {
                        panelDiv.classList.add("panel-default");
                    } else if (tempJson[key].statusFlag & statusFlags.DISCONNECTED) {
                        panelDiv.classList.add("panel-danger");
                    } else if (tempJson[key].statusFlag & statusFlags.INVALID) {
                        panelDiv.classList.add("panel-warning");
                    }
                    var headerDiv = document.createElement('div');
                    headerDiv.classList.add("panel-heading");
                    headerDiv.insertAdjacentHTML('afterBegin', '<h3 class="panel-title">Temperature #' + key + '</h3></div>');
                    var bodyDiv = document.createElement('div');
                    bodyDiv.classList.add("panel-body");
                    bodyDiv.insertAdjacentHTML('afterBegin', '<h1 id="body-temperature' + key + '" class="text-center main">' + tempJson[key].temperature + ' &deg;C</h1></div>');
                    var container = document.getElementById("panel-container");
                    panelDiv.appendChild(headerDiv);
                    panelDiv.appendChild(bodyDiv);
                    colDiv.appendChild(panelDiv);
                    container.appendChild(colDiv);
                });
            }
        }
    }; 

}
//DIRTY FIXME

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
//DIRTY FIXME
	initTemperature();
	updateState();
	setInterval(updateState, 5000);
//DIRTY FIXME	
	initWS();
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);