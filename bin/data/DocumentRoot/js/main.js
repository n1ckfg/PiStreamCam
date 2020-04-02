"use strict";

var liveView;
var hostname="127.0.0.1";
var port=7111;

function main() {
	loadFile("./js/hostname");
}

function loadFile(file) {
    var rawFile = new XMLHttpRequest();
    rawFile.open("GET", file, false);
    rawFile.onreadystatechange = function() {
        if (rawFile.readyState === 4) {
            if (rawFile.status === 200 || rawFile.status == 0) {
                hostname = rawFile.responseText;

                liveView = document.getElementById("live_view");
				liveView.src = "http://" + hostName + ".local:" + port;
            }
        }
    }
}

window.onload = main;
