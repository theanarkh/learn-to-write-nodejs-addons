const { makeCallback } = require('./build/Release/test.node');
makeCallback( (a,b) => {
    console.log(a,b)
}, {},1,2);
