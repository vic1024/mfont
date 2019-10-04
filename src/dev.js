'use strict'
let mfont = require('./mfont');
// console.log(mfont.installFont(`edd4074d83f3c5962d2f175d8300ccb8.ttf`, `test`));
// console.log(mfont.getError());
// console.log(mfont.installFont(`test_1.ttf`, `test_1`));
// console.log(mfont.getError());
// console.log(mfont.installFont(`test_2.ttf`, `test_2`));
// console.log(mfont.getError());
// console.log(mfont.removeFont(`edd4074d83f3c5962d2f175d8300ccb8.ttf`, `test`));
// console.log(mfont.getError());
// console.log(mfont.removeFont(`test_1.ttf`, `test_1`));
// console.log(mfont.getError());
// console.log(mfont.removeFont(`test_2.ttf`, `test_2`));
// console.log(mfont.getError());

// console.log(mfont.removeFont(`mh_Courier.ttf`, `mh_Courier`));
// console.log(mfont.getError());

console.log(`mh_Courier => ` + mfont.hasFont(`mh_Courier`));
console.log(mfont.getError());

console.log(`宋体 => ` + mfont.hasFont(`宋体`));
console.log(mfont.getError());

console.log(`SimSun => ` + mfont.hasFont(`SimSun`));
console.log(mfont.getError());

console.log(`Calibri => ` + mfont.hasFont(`Calibri`));
console.log(mfont.getError());

console.log(`Test12345 => ` + mfont.hasFont(`Test12345`));
console.log(mfont.getError());

console.log(`Small Fonts => ` + mfont.hasFont(`Small Fonts`));
console.log(mfont.getError());

console.log(`Small Fonts 12 => ` + mfont.hasFont(`Small Fonts 12`));
console.log(mfont.getError());