const { getPromise, isPromise } = require('./build/Release/test.node');

getPromise().then((result) => {
    console.log('resolve', result)
}, (result) => {
    console.log('reject',result)
});
