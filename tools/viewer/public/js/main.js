"use strict";

var camNameBegin = "ws://";
var camNameList = [ "nfg-rpi-3-4", "nfg-rpi-3-3", "nfg-rpi-3-2", "nfg-rpi-3-1", "nfg-rpi-2-2", "nfg-rpi-2-1", "nfg-rpi-0-1" ];
var camNameEnd = ".local:7112";
var camUrls = [];
var camWs = [];
var stillBoxes = [];

function main() {
	makeCamUrls();
	openCamConnections();
	setupKeys();

	for (var i=0; i<camNameList.length; i++) {
		var sb = document.getElementById("sb"+(i+1));
		stillBoxes.push(sb);
	}
}

window.onload = main;

// ~ ~ ~ ~ ~ ~ ~ ~ 

function setupKeys() {
	window.addEventListener("keydown", function(event) {
		//  
    });

    window.addEventListener("keyup", function(event) {
        if (getKeyCode(event) == ' ') takePhoto();
    });
}

function getKeyCode(event) {
    var k = event.charCode || event.keyCode;
    var c = String.fromCharCode(k).toLowerCase();
    return c;
}

function takePhoto() {
	console.log("Taking photos...");
	for (var i=0; i<camWs.length; i++) {
		camWs[i].send("Hello");
	}
}

function makeCamUrls() {
	for (var i=0; i<camNameList.length; i++) {
		var camUrl = camNameBegin + camNameList[i] + camNameEnd;

		camUrls.push(camUrl);
	}
}

function openCamConnections() {
	for (var i=0; i<camUrls.length; i++) {
		console.log("Attempting to open " + camUrls[i]);
		try {
			var ws = new WebSocket(camUrls[i]);
			camWs.push(ws);
		} catch (e) { }
	}	

	for (var i=0; i<camWs.length; i++) {
		camWs[i].onmessage = function(evt) { onMessage(evt) };
	}
}

// ~ ~ ~ ~ ~ ~ ~ ~ 

function onMessage(evt) {
	var results = evt.data.split(",");
	console.log(results);
	for (var i=0; i<camNameList.length; i++) {
		if (results[0] == camNameList[i]) {
			var url = "http://" + camNameList[i] + ".local:7110/photos/" + results[1];
			console.log("RESPONSE: " + url);
			
			var filename = url.split("/")[url.split("/").length-1];
			download(filename, url);

			stillBoxes[i].style.backgroundImage = "url(\"" + url + "\")";
			stillBoxes[i].style.backgroundSize = "100px 75px";	

			break;
		}
	}
}

function download(filename, url) {
    var element = document.createElement('a');
    //element.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
    element.setAttribute('href', url);
    element.setAttribute('download', filename);
    element.setAttribute('target', '_blank');
    element.style.display = 'none';
    document.body.appendChild(element);

    element.click();

    document.body.removeChild(element);
}


/*
var wsUrl = "ws://nfg-rpi-3-4.local:7112";
var websocket;

function setupWs() {
	websocket = new WebSocket(wsUrl);
	websocket.onopen = function(evt) { onOpen(evt) };
	websocket.onclose = function(evt) { onClose(evt) };
	websocket.onmessage = function(evt) { onMessage(evt) };
	websocket.onerror = function(evt) { onError(evt) };
}

function onOpen(evt) {
	console.log("CONNECTED");
	doSend("Hello");
}

function onClose(evt) {
	console.log("DISCONNECTED");
}

function onMessage(evt) {
	console.log("RESPONSE: " + evt.data);
	websocket.close();
}

function onError(evt) {
	console.log("ERROR: " + evt.data);
}

function doSend(message) {
	console.log("SENT: " + message);
	websocket.send(message);
}
*/