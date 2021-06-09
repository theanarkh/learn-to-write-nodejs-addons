const net = require('net');

function handle() {
    setTimeout(() => {
        const socket = net.connect({host: '127.0.0.1', port: 8989 });
        socket.on('connect', () => {
            console.log('ok');
            socket.destroy();
            handle();
        });
    }, 1000);
}

handle();