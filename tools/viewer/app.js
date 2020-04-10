"use strict";

const express = require("express");
const app = express();
const request = require('request');
const fs = require("fs");
const dotenv = require("dotenv").config();
const debug = process.env.DEBUG === "true";

var options;
if (!debug) {
	options = {
	    key: fs.readFileSync(process.env.KEY_PATH),
	    cert: fs.readFileSync(process.env.CERT_PATH)
	};
}

const https = require("https").createServer(options, app);

// default -- pingInterval: 1000 * 25, pingTimeout: 1000 * 60
// low latency -- pingInterval: 1000 * 5, pingTimeout: 1000 * 10
var io, http;
const ping_interval = 1000 * 5;
const ping_timeout = 1000 * 10;
const port_http = process.env.PORT_HTTP;
const port_https = process.env.PORT_HTTPS;
const port_ws = process.env.PORT_WS;

const WebSocket = require("ws");
const ws = new WebSocket.Server({ port: port_ws, pingInterval: ping_interval, pingTimeout: ping_timeout }, function() {
    console.log("\nNode.js listening on websocket port " + port_ws);
});

if (!debug) {
    http = require("http");

    http.createServer(function(req, res) {
        res.writeHead(301, { "Location": "https://" + req.headers['host'] + req.url });
        res.end();
    }).listen(port_http);

    io = require("socket.io")(https, { 
        pingInterval: ping_interval,
        pingTimeout: ping_timeout
    });
} else {
    http = require("http").Server(app);

    io = require("socket.io")(http, { 
        pingInterval: ping_interval,
        pingTimeout: ping_timeout
    });
}
   
app.use(express.static("public")); 

app.get("/", function(req, res) {
    res.sendFile(__dirname + "/public/index.html");
});

if (!debug) {
    https.listen(port_https, function() {
        console.log("\nNode.js listening on https port " + port_https);
    });
} else {
    http.listen(port_http, function() {
        console.log("\nNode.js listening on http port " + port_http);
    });
}

io.on("connection", function(socket) {
    console.log("A socket.io user connected.");

    var url = "http://nfg-rpi-3-4.local:7110/photos/nfg-rpi-3-4_2020-04-08-22-10-43-354_Q100.jpg";
    download(url, "file.jpg", function(response) {
        console.log("Download complete.");
    });

    socket.on("disconnect", function(event) {
        console.log("A socket.io user disconnected.");
    });
});

ws.on("connection", function(socket) {
    console.log("A user connected.");

    socket.onclose = function(event) {
        console.log("A user disconnected.");
    };

    socket.onmessage = function(event) {
        //
    };
});

// https://stackoverflow.com/questions/11944932/how-to-download-a-file-with-node-js-without-using-third-party-libraries
function download(url, dest, cb) {
    const file = fs.createWriteStream(dest);
    const sendReq = request.get(url);

    // verify response code
    sendReq.on('response', (response) => {
        if (response.statusCode !== 200) {
            return cb('Response status was ' + response.statusCode);
        }

        sendReq.pipe(file);
    });

    // close() is async, call cb after close completes
    file.on('finish', () => file.close(cb));

    // check for request errors
    sendReq.on('error', (err) => {
        fs.unlink(dest);
        return cb(err.message);
    });

    file.on('error', (err) => { // Handle errors
        fs.unlink(dest); // Delete the file async. (But we don't check the result)
        return cb(err.message);
    });
};