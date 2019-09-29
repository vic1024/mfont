'use strict'
let mfont = require('./mfont');
console.log(mfont.installFont(`edd4074d83f3c5962d2f175d8300ccb8.ttf`, `test`));
console.log(mfont.getError());
console.log(mfont.installFont(`test_1.ttf`, `test_1`));
console.log(mfont.getError());
console.log(mfont.installFont(`test_2.ttf`, `test_2`));
console.log(mfont.getError());
console.log(mfont.removeFont(`edd4074d83f3c5962d2f175d8300ccb8.ttf`, `test`));
console.log(mfont.getError());
console.log(mfont.removeFont(`test_1.ttf`, `test_1`));
console.log(mfont.getError());
console.log(mfont.removeFont(`test_2.ttf`, `test_2`));
console.log(mfont.getError());
