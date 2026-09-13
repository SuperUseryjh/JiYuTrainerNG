{
  "targets": [
    {
      "target_name": "jiyu_native",
      "sources": [
        "src/addon.cpp",
        "native/jiyu_common.cpp",
        "native/unlock.cpp",
        "native/monitor.cpp",
        "native/udp.cpp",
        "native/knock.cpp",
        "native/driver.cpp",
        "native/inject.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "native"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": [ "UNICODE", "_UNICODE" ],
      "cflags_cc": [ "-std=c++17" ],
      "msvs_settings": {
        "VCCLCompilerTool": {
          "ExceptionHandling": 1,
          "AdditionalOptions": [ "/utf-8", "/std:c++17" ]
        }
      },
      "conditions": [
        [ "OS=='win'", {
          "libraries": [ "-lws2_32.lib" ]
        } ]
      ]
    }
  ]
}
