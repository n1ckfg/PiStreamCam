"use strict";

const width = 640;
const height = 480;
const port = 7111;

function loadLiveView() {
	document.getElementById("live_view").innerHTML='<object type="text/html" width="' + width + '" height="' + height + '" data="http://127.0.0.1:' + port + '" ></object>';
}

window.onload = loadLiveView;