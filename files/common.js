'use strict';

//wsBinProtocol constants
const wsBinConst = {
//Frame header offsets
	wsCmd			: 0, //Command type
	wsSysId			: 1, //target sysId
	wsSubCmd		: 2, //sub-command type
	wsPayLoadStart	: 3,
	//alternatively if we need argument to Get value
	wsPayLoadStartGetSetArg	: 4,
	wsGetSetArg		: 3,

	reservedCmd		: 0,
	getCmd			: 1,
	setCmd			: 2,
	getResponse		: 3,
	setAck			: 4,
	setNack			: 5,

// sub-command
	scAppSetTime	: 1,
	scAppGetStatus	: 2,
// sub-commands for BinStateHttpClass sysId=2 and BinStatesHttpClass sysId=3
	scBinStateGetName	: 1,
	scBinStateGetState	: 2,
	scBinStateSetState	: 3,
// sub-commands for BinStatesHttpClass sysId=3
	scBinStatesGetAll	: 1,
// BinHttp State/Buttons base numbers
	uidHttpState		: 0,
	uidHttpButton		: 127
};


const wsBinCmd = {
	Get	: function (socket, sysId, subCmd) {
		var ab = new ArrayBuffer(3);
		var bin = new DataView(ab);
		
		bin.setUint8(wsBinConst.wsCmd, wsBinConst.getCmd);
		bin.setUint8(wsBinConst.wsSysId, sysId);
		bin.setUint8(wsBinConst.wsSubCmd, subCmd);
	
		socket.send(bin.buffer);
		console.log.bind(console)(`wsGet sysId = ${sysId}, subCmd = ${subCmd}`);
	},
	SetArg: function (socket, sysId, subCmd, setArg, setValue) {
		var ab = new ArrayBuffer(5);
		var bin = new DataView(ab);
		
		bin.setUint8(wsBinConst.wsCmd, wsBinConst.setCmd);
		bin.setUint8(wsBinConst.wsSysId, sysId);
		bin.setUint8(wsBinConst.wsSubCmd, subCmd);
		bin.setUint8(wsBinConst.wsGetSetArg, setArg);
		bin.setUint8(wsBinConst.wsPayLoadStartGetSetArg, setValue);
	
		socket.send(bin.buffer);
		console.log.bind(console)(`wsGet sysId = ${sysId}, subCmd = ${subCmd}`);
	}
}

//BinStateHttpClass

function BinStateClass (uid) {
	this.uid = uid;
	this._state = 0; //false
	this._name = "";
	this._initDone = false;
	
	// this.wsGetName();
	// this.wsGetState();
}

BinStateClass.prototype.wsGet = function (cmd) {
	var ab = new ArrayBuffer(3);
	var bin = new DataView(ab);
	
	bin.setUint8(wsBinConst.wsCmd, wsBinConst.getCmd);
	bin.setUint8(wsBinConst.wsSysId, 2); //BinStateHttpClass.sysId = 2
	bin.setUint8(wsBinConst.wsSubCmd, cmd);

	websocket.send(bin.buffer);
	console.log.bind(console)(`wsGet cmd = ${cmd}`);
}

BinStateClass.prototype.wsGetName = function () {
	this.wsGet(1);
}

BinStateClass.prototype.wsGetState = function () {
	this.wsGet(2);
}

BinStateClass.prototype.wsSetState = function (state) {
	wsBinCmd.SetArg(websocket, 3, wsBinConst.scBinStateSetState, this.uid, state);
}

BinStateClass.prototype.wsGotName = function (bin) {
	var uid = bin.getUint8(wsBinConst.wsPayLoadStart, true);
	var strBuffer = new Uint8Array(bin.byteLength);
    for (var i = 0; i < strBuffer.length - (wsBinConst.wsPayLoadStart + 1); i++) {
        strBuffer[i] = bin.getUint8(wsBinConst.wsPayLoadStart + 1 + i);
//        console.log.bind(console)(`uid = ${uid}, strBuffer[${i}] = ${bin.getUint8(wsBinConst.wsPayLoadStart + 1 + i)}`);
    }
    this._name = new TextDecoder().decode(strBuffer)
//    console.log.bind(console)(`uid = ${uid}, name = ${this._name}`);
    
    if ( !this._initDone ) {
    	this._initDone = true;
	   	var t = document.querySelector('#BinStateHttpClass');
	  
		var clone = document.importNode(t.content, true);
	  	clone.querySelector('#binState').textContent = `${this._name}`;
	  	clone.querySelector('#binStatePanel').id = `binStatePanel${this.uid}`;
		
		
//temparary for BinHttpButton
		clone.querySelector('#binState').addEventListener('mousedown', this);
		clone.querySelector('#binState').addEventListener('mouseup', this);
//temporary for BinHttpButton

		clone.querySelector('#binState').id = `binState${this.uid}`;
				
		var container = document.getElementById("panel-container");
		container.appendChild(clone);	
    }	
}

BinStateClass.prototype.wsGotState = function (bin) {
	var uid = bin.getUint8(wsBinConst.wsPayLoadStart, true);
	this._state = bin.getUint8(wsBinConst.wsPayLoadStart + 1, true);
//    console.log.bind(console)(`name = {this._name}, uid = ${uid}, state = ${this._state}`);
    
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

BinStateClass.prototype.handleEvent = function(event) {
	switch(event.type) {
		case 'mousedown':
	        this.wsSetState(1);
	        break;
        case 'mouseup':
            this.wsSetState(0);
            break;
	}
}

//BinStatesHttpClass

function BinStatesClass () {
	this._binStatesHttp = {};
	this.wsGetAll();
}

BinStatesClass.prototype.wsGetAll = function() {
	wsBinCmd.Get(websocket, 3, wsBinConst.scBinStatesGetAll);
}

BinStatesClass.prototype.wsBinProcess = function (bin) {
	var subCmd = bin.getUint8(wsBinConst.wsSubCmd);
	var uid = bin.getUint8(wsBinConst.wsPayLoadStart);
	
	if (subCmd == wsBinConst.scBinStateGetName) {
		if ( !this._binStatesHttp.hasOwnProperty(uid) ) {
			this._binStatesHttp[uid] = new BinStateClass(uid);
			this._binStatesHttp[uid].wsGotName(bin);
		}
	}
	
	if (subCmd == wsBinConst.scBinStateGetState) {
		this._binStatesHttp[uid].wsGotState(bin);
	}
	
}