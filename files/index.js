'use strict';

const statusFlags = { INVALID: 1, DISCONNECTED: 2};

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

//Here we put some initial code which starts after DOM loaded
function onDocumentRedy() {
	//Init
	updateState();
	setInterval(updateState, 5000);
}

document.addEventListener('DOMContentLoaded', onDocumentRedy);