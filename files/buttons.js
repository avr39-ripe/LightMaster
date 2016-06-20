'use strict';

function getJson(url) {
	return fetch(url).then(function(response){if (response.status >= 200 && response.status < 300) return response.json();});
};

function post_button(key,state) {
	var json = {};
	json["state"] = state;
	
	fetch(`/button?button=${key}`, {
		method: 'post',
		headers: {
			'Accept': 'application/json',
			'Content-Type': 'application/json; charset=utf-8'
			},
		body: JSON.stringify(json)
	});
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
		container.insertAdjacentHTML("beforeEnd",`<br><div class="col-xs-10 col-md-5"><button id="Button${key}" class="btn btn-primary btn-block">${Json[key].name}</button></div>`);
		var button = document.getElementById(`Button${key}`);
		button.addEventListener('mousedown', function() { post_button(key, 1); });
		button.addEventListener('mouseup', function() { post_button(key, 0); });
		updateButton(key,Json[key].state);
	})})
}

function updateButtons() {
	getJson("/button")
	.then(function(Json){Object.keys(Json).forEach(function(key){
		updateButton(key,Json[key].state);
	})})
}

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	//Init
	initButtons();
//	updateButtons();
	setInterval(updateButtons, 3000);

}

document.addEventListener('DOMContentLoaded', onDocumentRedy);