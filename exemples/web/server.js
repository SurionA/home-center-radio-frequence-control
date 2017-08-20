const http = require('http');
const fs = require('fs');
const server = http.createServer((req, res) => {
    fs.readFile('./index.html', 'utf-8', function(error, content) {
        res.writeHead(200, {"Content-Type": "text/html"});
        res.end(content);
    });
});
const io = require('socket.io').listen(server);

server.listen(3000);
const radioFrequenceControl = require('../../index');

radioFrequenceControl.initSocket(io);
