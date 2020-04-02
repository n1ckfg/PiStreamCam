"use strict";

const width = 640;
const height = 480;
const hostName = "nfg-rpi-3-4.local";
const port = 7111;

var liveView;

function main() {
	liveView = document.getElementById("live_view");
}

window.onload = main;