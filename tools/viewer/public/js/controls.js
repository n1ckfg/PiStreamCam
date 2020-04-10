"use strict";

function setupKeys() {
	window.addEventListener("keydown", function(event) {
		//  
    });

    window.addEventListener("keyup", function(event) {
        if (getKeyCode(event) == ' ') {
        	takePhoto();
        } else if (getKeyCode(event) == 's') {
		 	for (var i=0; i<camWs.length; i++) {
		 		slowVideo = !slowVideo;
		 		if (slowVideo) {
					camWs[i].send("update_slow");
				} else {
					camWs[i].send("update_fast");
				}
			}       	
        }
    });
}

function getKeyCode(event) {
    var k = event.charCode || event.keyCode;
    var c = String.fromCharCode(k).toLowerCase();
    return c;
}