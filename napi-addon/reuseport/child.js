const { socket } = require('./build/Release/test.node');
const net = require('net');
const fd = socket();
net.createServer(() => {
    console.log(process.pid);
}).listen({fd});
