const { FileReader } = require('./FileReader');
const fs = require('fs');
const fd = fs.openSync('./io_uring.cc');
const fileRead = new FileReader({fd});
fileRead.on('data', (buffer) => {
    console.log(buffer.toString('utf-8'));
})
