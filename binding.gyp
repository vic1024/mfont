{
  "targets": [
    {
      "target_name": "mfont",
      'sources': ['src/mfont.cc'],
      "defines": ["NAPI_CPP_EXCEPTIONS"],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "xcode_settings": {
        "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
      }
    }
  ]
}