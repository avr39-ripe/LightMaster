'use strict';

function get_config() {
    fetch('/config.json')
  	.then(function(response) {
      if (response.status >= 200 && response.status < 300) return response.json();
	})
	.then(function(configJson) {
		Object.keys(configJson).forEach(function(key) {
			document.getElementById(key).value = configJson[key];
		});
		if (configJson.StaEnable == 1) {
			document.getElementById('StaEnable').checked = true;
		} else {
			document.getElementById('StaEnable').checked = false;
		}
	});
}

function post_cfg(jsonData) {
	fetch('/config', {
		method: 'post',
		headers: {
			'Accept': 'application/json',
			'Content-Type': 'application/json; charset=utf-8'
			},
		body: JSON.stringify(jsonData)
	});
}

function post_netcfg(event) {
	event.preventDefault();
    var formData = {
            'StaSSID'                    :   document.getElementById('StaSSID').value,
            'StaPassword'                :   document.getElementById('StaPassword').value,
            'StaEnable'                  :   (document.getElementById('StaEnable').checked ? 1 : 0)
            };
    post_cfg(formData);
}

function post_config(event) {
	event.preventDefault();
	var formData = {
			'loopInterval'			:	document.getElementById('loopInterval').value,
			'updateURL'				:	document.getElementById('updateURL').value
			};
	post_cfg(formData);
}

function post_fw(action) {
// action should be either "update" or "switch"
	var json = {};
	json[action] = 1;
	
	fetch('/update', {
		method: 'post',
		headers: {
			'Accept': 'application/json',
			'Content-Type': 'application/json; charset=utf-8'
			},
		body: JSON.stringify(json)
	});
}

//Websockets
var websocket;
function onOpen(evt) {
	console.log.bind(console)("CONNECTED");
	wsGetRandom();
//	websocket.send("Sming love WebSockets");
}

function onClose(evt) {
	console.log.bind(console)("DISCONNECTED");
}

function onMessage(evt) {
	console.log.bind(console)("Message recv: " + evt.data);
	var json = JSON.parse(evt.data);
	console.log.bind(console)("Json recv: " + json);
	
	if (json.response == "getRandom") {
		onGetRandom(json);
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

function wsGetRandom() {
	var json = {};
	json["command"] = "getRandom";
	websocket.send(JSON.stringify(json));
}

function onGetRandom(json) {
	Object.keys(json).forEach(function(key) {
		if(key != "response") {
				document.getElementById(key).value = json[key];
		}
	});
}

function sendRandom(event) {
	event.preventDefault();
	var json = {};
	json["command"] = "setRandom";
	
	json["startTime"] = document.getElementById('startTime').value;
	json["stopTime"] = document.getElementById('stopTime').value;
	json["minOn"] = document.getElementById('minOn').value;
	json["maxOn"] = document.getElementById('maxOn').value;
	json["minOff"] = document.getElementById('minOff').value;
	json["maxOff"] = document.getElementById('maxOff').value;
	
	websocket.send(JSON.stringify(json));
}

function sendTime(event) {
	event.preventDefault();
	var ab = new ArrayBuffer(6);
	var bin = new DataView(ab);
	var d = new Date();
	bin.setUint8(0,42);
	bin.setUint32(1,Math.round(d.getTime() / 1000),true);
	bin.setUint8(5, Math.abs(d.getTimezoneOffset()/60));	
//	console.log.bind(console)(bin.getUint8(1),bin.getUint8(2),bin.getUint8(3),bin.getUint8(4));
	websocket.send(bin.buffer);
}

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
    //Init
    initWS();
    get_config();
    
    document.getElementById('form_netcfg').addEventListener('submit', post_netcfg);
    document.getElementById('netcfg_cancel').addEventListener('click', get_config);
    document.getElementById('form_settings').addEventListener('submit', post_config);
	document.getElementById('settings_cancel').addEventListener('click', get_config);
	document.getElementById('settings_update_fw').addEventListener('click', function() { post_fw("update"); });
	document.getElementById('settings_switch_fw').addEventListener('click', function() { post_fw("switch"); });
	document.getElementById('form_random').addEventListener('submit', sendRandom);
	document.getElementById('random_cancel').addEventListener('click', wsGetRandom);
	document.getElementById('sync_datetime').addEventListener('click', sendTime);

}

document.addEventListener('DOMContentLoaded', onDocumentRedy);
