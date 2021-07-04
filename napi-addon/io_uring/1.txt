const { FileReader } = require('./FileReader');
const fs = require('fs');
const fd = fs.openSync('./1.txt');
const fileRead = new FileReader({fd});
fileRead.on('data', (buffer) => {
    console.log(buffer);
})
