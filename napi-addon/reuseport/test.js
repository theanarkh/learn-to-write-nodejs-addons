const child_process = require('child_process');
for (let i = 0; i < 5; i++) {
    child_process.fork(`${__dirname}/child.js`);
}