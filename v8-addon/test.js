const { Demo, genSeq } = require('./build/Release/test.node');
const demo = new Demo();
console.log('demo对象：', demo, '\n');
console.log('原型属性：', demo.protoField, '\n');
console.log('执行add方法：', demo.add(1,2), '\n');  
console.log('执行seq方法：', genSeq(), genSeq(), '\n');   