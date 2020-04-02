"use strict";

var liveView;
var hostname="nfg-rpi-3-4.local";
var port=7111;

function main() {
	hostname = loadFile("hostname");

	liveView = document.getElementById("live_view");
	liveView.src = "http://" + hostName + ":" + port;

	alert(hostname);
}

function loadFile(filePath) {
  var result = null;
  var xmlhttp = new XMLHttpRequest();
  xmlhttp.open("GET", filePath, false);
  xmlhttp.send();
  if (xmlhttp.status==200) {
    result = xmlhttp.responseText;
  }
  return result;
}

window.onload = main;
