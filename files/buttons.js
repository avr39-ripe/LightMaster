'use strict';

// function updateState() {
	// fetch('/state.json')
  	// .then(function(response) {
      // if (response.status == 200) return response.json();
   // })
  // .then(function(json) {
    // document.getElementById('counter').textContent = json.counter;
  // });
// }
function getJson(url) {
	return fetch(url).then(function(response){if (response.status >= 200 && response.status < 300) return response.json();});
};

// function updateState() {
    // var xhr = new XMLHttpRequest();
    // var xhr1 = new XMLHttpRequest();
//     
    // xhr1.open('GET', '/state.json', true);
// 
    // xhr1.send();
// 
    // xhr1.onreadystatechange = function() {
//         
        // if (this.readyState != 4) return;
        // if (this.status == 200) {
            // if (this.responseText.length > 0) {
                // var stateJson = JSON.parse(this.responseText);
                // document.getElementById('counter').textContent = stateJson.counter;
            // }
        // }
    // };
//     
    // xhr.open('GET', '/temperature.json', true);
// 
    // xhr.send();
// 
    // xhr.onreadystatechange = function() {
//         
        // if (this.readyState != 4) return;
        // if (this.status == 200) {
            // if (this.responseText.length > 0) {
                // var tempJson = JSON.parse(this.responseText);
                // Object.keys(tempJson).forEach(function(key) {
                    // var panelDiv = document.getElementById('panel-temperature' + key);
                    // panelDiv.className = '';
                    // panelDiv.classList.add("panel");
                    // if (tempJson[key].statusFlag == 0) {
                        // panelDiv.classList.add("panel-default");
                    // } else if (tempJson[key].statusFlag & statusFlags.DISCONNECTED) {
                        // panelDiv.classList.add("panel-danger");
                    // } else if (tempJson[key].statusFlag & statusFlags.INVALID) {
                        // panelDiv.classList.add("panel-warning");
                    // }
                    // var bodyDiv = document.getElementById('body-temperature' + key);
                    // bodyDiv.innerHTML = tempJson[key].temperature + ' &deg;C';
               // });
            // }
        // }
   // };
// }

function initButtons() {
	getJson("http://10.2.113.121/button")
	.then(function(Json){ Object.keys(Json).forEach(function(key){
		var container = document.getElementById("panel-container");
		container.insertAdjacentHTML("beforeEnd",`<div class="col-xs-10 col-md-5"><button id="Button${key}" class="btn btn-primary btn-block">${Json[key].name}</button></div>`);
	};};))
}
                // Object.keys(tempJson).forEach(function(key) {
                    // var colDiv = document.createElement('div');
                    // colDiv.classList.add("col-xs-10");
                    // colDiv.classList.add("col-md-5");
                    // var panelDiv = document.createElement('div');
                    // panelDiv.classList.add("panel");
                    // panelDiv.id = "panel-temperature" + key;
                    // if (tempJson[key].statusFlag == 0) {
                        // panelDiv.classList.add("panel-default");
                    // } else if (tempJson[key].statusFlag & statusFlags.DISCONNECTED) {
                        // panelDiv.classList.add("panel-danger");
                    // } else if (tempJson[key].statusFlag & statusFlags.INVALID) {
                        // panelDiv.classList.add("panel-warning");
                    // }
                    // var headerDiv = document.createElement('div');
                    // headerDiv.classList.add("panel-heading");
                    // headerDiv.insertAdjacentHTML('afterBegin', '<h3 class="panel-title">Temperature #' + key + '</h3></div>');
                    // var bodyDiv = document.createElement('div');
                    // bodyDiv.classList.add("panel-body");
                    // bodyDiv.insertAdjacentHTML('afterBegin', '<h1 id="body-temperature' + key + '" class="text-center main">' + tempJson[key].temperature + ' &deg;C</h1></div>');
                    // var container = document.getElementById("panel-container");
                    // panelDiv.appendChild(headerDiv);
                    // panelDiv.appendChild(bodyDiv);
                    // colDiv.appendChild(panelDiv);
                    // container.appendChild(colDiv);
                // });
            // }
        // }
    // }; 
//}

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	//Init
	initButtons();
//	updateState();
//	setInterval(updateState, 5000);

}

document.addEventListener('DOMContentLoaded', onDocumentRedy);