"use strict";

var camNameBegin = "ws://nfg-rpi-";
var camNameList = [ "3-4", "3-3", "3-2", "3-1", "2-2", "2-1", "0-1" ];
var camNameEnd = ".local:7112";
var camUrls = [];
var camWs = [];
var stillBoxes = [];

function main() {
	makeCamUrls();
	openCamConnections();
	setupKeys();

	for (var i=0; i<camNameList; i++) {
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
}

// ~ ~ ~ ~ ~ ~ ~ ~ 

var wsUrl = "ws://nfg-rpi-3-4.local:7112";

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
