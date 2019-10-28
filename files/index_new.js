'use strict';

//var appStatus
//var binStates;
//var tempsensors;
//var tempsensorsHome;

//import websocket from './websocket';
//import AppStatusClass from 'appStatus';
import BinStatesClass from 'binStates';
import { initWS, websocket, wsEnablers, wsBinProcessors } from 'websocket';
//import TempsensorsClass from 'tempsensors';

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
//	tempsensors = new TempsensorsClass('/temperature.json', 0);
//	tempsensors.enable(true);
//	setInterval(function () { tempsensors.wsGetAllTemperatures(); }, 5000);
//	
//	tempsensorsHome = new TempsensorsClass('/temperatureHome.json', 1);
//	tempsensorsHome.enable(true);
//	setInterval(function () { tempsensorsHome.wsGetAllTemperatures(); }, 5000);

//	var appStatus = new AppStatusClass();
	var binStates = new BinStatesClass();
	
	
//	wsEnablers.push(appStatus.enable.bind(appStatus));
	wsEnablers.push(binStates.enableStates.bind(binStates));
	wsEnablers.push(binStates.enableButtons.bind(binStates));
	
//	wsBinProcessors[AppStatusClass.sysId] = appStatus.wsBinProcess.bind(appStatus);
	wsBinProcessors[BinStatesClass.sysId] = binStates.wsBinProcess.bind(binStates);
	
	initWS();
	
	// Handle different menu items
	document.getElementById('menuShutters').addEventListener('click', () => { binStates.showOnlyUids([0, 152, 155,156,157,158,159,160]);});
	document.getElementById('menuLight').addEventListener('click', () => { binStates.showOnlyUids([0, 127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,152,153,154,161,162]);});
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);