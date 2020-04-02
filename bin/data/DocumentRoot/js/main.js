"use strict";

const width = 640;
const height = 480;
const port = 7111;

var liveView;

function main() {
	liveView = document.getElementById("live_view");

	updateLiveView();
}

function updateLiveView() {
	liveView.innerHTML='<object type="text/html" width="' + width + '" height="' + height + '" data="http://localhost:' + port + '" ></object>';
}

window.onload = main;