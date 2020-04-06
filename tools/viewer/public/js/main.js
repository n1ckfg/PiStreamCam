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
			
			loadToCanvas(url);

			stillBoxes[i].style.backgroundImage = "url(\"" + url + "\")";
			stillBoxes[i].style.backgroundSize = "100px 75px";	

			break;
		}
	}
}

function loadToCanvas(url) {
	var img = new Image;
	var c = document.createElement("canvas");
	var ctx = c.getContext("2d");

	img.onload = function() {
	  c.width = this.naturalWidth;     // update canvas size to match image
	  c.height = this.naturalHeight;
	  ctx.drawImage(this, 0, 0);       // draw in image
	  c.toBlob(function(blob) {        // get content as JPEG blob
	    // here the image is a blob
	    downloadBlob(blob, "test.jpg");
	  }, "image/jpeg", 1.0);
	};
	img.crossOrigin = "";              // if from different origin
	img.src = url;
}

function downloadBlob(blob, filename) {
  // Create an object URL for the blob object
  const url = URL.createObjectURL(blob);
  
  // Create a new anchor element
  const a = document.createElement('a');
  
  // Set the href and download attributes for the anchor element
  // You can optionally set other attributes like `title`, etc
  // Especially, if the anchor element will be attached to the DOM
  a.href = url;
  a.download = filename || 'download';
  
  // Click handler that releases the object URL after the element has been clicked
  // This is required for one-off downloads of the blob content
  const clickHandler = () => {
    setTimeout(() => {
      URL.revokeObjectURL(url);
      this.removeEventListener('click', clickHandler);
    }, 150);
  };
  
  // Add the click event listener on the anchor element
  // Comment out this line if you don't want a one-off download of the blob content
  a.addEventListener('click', clickHandler, false);
  
  // Programmatically trigger a click on the anchor element
  // Useful if you want the download to happen automatically
  // Without attaching the anchor element to the DOM
  // Comment out this line if you don't want an automatic download of the blob content
  a.click();
  
  // Return the anchor element
  // Useful if you want a reference to the element
  // in order to attach it to the DOM or use it in some other way
  return a;
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