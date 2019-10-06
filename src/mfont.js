'use strict'
const os = require('os')

var mfont = {
    error: '',
    getError: function () {
        return this.error;
    },
    hasFont: function (fontname) {
        return "1";
    },
    installFont: function (filename, fontname) {
        return "1";
    },
    removeFont: function (filename, fontname) {
        return "1";
    }
}

if (os.platform() === 'win32') {
    const font = require('bindings')('../build/Release/mfont');
    mfont.hasFont = function (fontname) {
        if (fontname === undefined || fontname === null || fontname === '') {
            this.error = '字体名称不能为空'
            return '0';
        }
        let result = font.hasFont(fontname);
        this.error = font.getError();
        return result;
    }
    mfont.installFont = function (filename, fontname) {
        if (fontname === undefined || fontname === null || fontname === '') {
            fontname = filename.substr(0, filename.lastIndexOf('.'));
        }
        let result = font.installFont(filename, fontname);
        this.error = font.getError();
        return result;
    }
    mfont.removeFont = function (filename, fontname) {
        if (fontname === undefined || fontname === null || fontname === '') {
            fontname = filename.substr(0, filename.lastIndexOf('.'));
        }
        let result = font.removeFont(filename, fontname);
        this.error = font.getError();
        return result;
    }
// } else if (os.platform() === 'linux') {
// } else if (os.platform() === 'darwin') {
}

module.exports = mfont
