const { Base, Derived } = require('./build/Release/test.node');
const base = new Base();
const derived = new Derived();
base.print();
derived.hello();
derived.print();
console.log(derived instanceof Base, derived instanceof Derived)