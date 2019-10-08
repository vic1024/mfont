'use strict'
const font = require('bindings')('../build/Release/mfont');
var mfont = {
    error: '',
    getError: function () {
        return this.error;
    },
    hasFont: function (fontname) {
        if (fontname === undefined || fontname === null || fontname === '') {
            this.error = '字体名称不能为空'
            return '0';
        }
        let result = font.hasFont(fontname);
        this.error = font.getError();
        return result;
    },
    installFont: function (filename, fontname) {
        if (fontname === undefined || fontname === null || fontname === '') {
            fontname = filename.substr(0, filename.lastIndexOf('.'));
        }
        let result = font.installFont(filename, fontname);
        this.error = font.getError();
        return result;
    },
    removeFont: function (filename, fontname) {
        if (fontname === undefined || fontname === null || fontname === '') {
            fontname = filename.substr(0, filename.lastIndexOf('.'));
        }
        let result = font.removeFont(filename, fontname);
        this.error = font.getError();
        return result;
    }
}

module.exports = mfont
