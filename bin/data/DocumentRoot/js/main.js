"use strict";

var liveView;
var hostname="127.0.0.1";
var port=7111;

function main() {
	hostname = loadFile("./js/hostname");

	liveView = document.getElementById("live_view");
	liveView.src = "http://" + hostName + ".local:" + port;

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
