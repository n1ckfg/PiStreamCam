"use strict";

var socket = io();

function sendFileList(fileList) {
    socket.emit("download_files", fileList);
}

/*
socket.on("receive_example", function(data) {
    var index = data[0]["index"];
    var last = layers.length - 1;
  	if (newStrokes.length > 0 && layers.length > 0 && layers[last].frames) layers[last].frames[index] = newStrokes;
});

function sendExample() {
    socket.emit("send_example", tempStrokeToJson());
}
*/
