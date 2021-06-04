const { submitWork } = require('./build/Release/test.node');
submitWork((sum) => {
    console.log(sum)
})
