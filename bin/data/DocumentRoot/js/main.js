"use strict";

var liveView;
var hostname="127.0.0.1";
var port=7111;

function main() {
	loadFile("./js/hostname", function(evt) {
	    hostname = evt.replace(/^\n|\n$/g, ''); // remove line breaks
        
        liveView = document.getElementById("live_view");
		liveView.src = "http://" + hostname + ".local:" + port;
	});
}

function loadFile(filepath, callback) { 
    // https://codepen.io/KryptoniteDove/post/load-json-file-locally-using-pure-javascript  
    var xobj = new XMLHttpRequest();
    xobj.overrideMimeType("text/plain");
    xobj.open('GET', filepath, true);
    xobj.onreadystatechange = function() {
        if (xobj.readyState == 4 && xobj.status == "200") {
            callback(xobj.responseText);
        }
    };
    xobj.send(null);  
}

window.onload = main;
