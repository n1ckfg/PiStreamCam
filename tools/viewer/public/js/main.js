"use strict";

var camNameBegin = "nfg-rpi-";
var camNameList = [ "0-1", "2-1", "2-2", "3-1", "3-2", "3-3", "3-4" ];
var camNameEnd = ".local:7110";
var activeCams = [];

function main() {
	loadPages();
	setupKeys();
}

window.onload = main;

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

// https://stackoverflow.com/questions/5211328/clicking-submit-button-of-an-html-form-by-a-javascript-code
function takePhoto() {
	console.log("Taking photos...");
	for (var i=0; i<activeCams.length; i++) {
		//
	}
}

// https://gomakethings.com/getting-html-asynchronously-from-another-page/
function getHtml(url, callback) {
	if (!window.XMLHttpRequest) return; // Feature detection

	var xhr = new XMLHttpRequest();

	xhr.onload = function() {
		if (callback && typeof(callback) === 'function' ) {
			callback(this.responseXML);
		}
	}

	xhr.open( 'GET', url );
	xhr.responseType = 'document';
	xhr.send();
}

function loadPages() {
	for (var i=0; i<camNameList.length; i++) {
		var camUrl = "http://" + camNameBegin + camNameList[i] + camNameEnd;
		console.log(camUrl);

		var newDoc = getHtml(camUrl, function(evt) {
			console.log(evt);
		});
	}
}

