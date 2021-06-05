const { isPromise } = require('./build/Release/test.node');

console.log('is Promise', isPromise(1), isPromise(Promise.resolve()));
