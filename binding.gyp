{
  "targets": [{
    "target_name"    : "metrohash",
    "sources"        : [ "./src/metrohash_wrapper.cpp", "./src/metrohash128.cpp", "./src/metrohash128crc.cpp", "./src/metrohash64.cpp" ],
    "cflags"         : [ "-std=c++11 -Wno-deprecated-declarations -Wno-unused-value -Wno-unused-function -Wno-unknown-pragmas -Wno-format -msse4.2" ],
    "include_dirs"   : [ "<!(node -e \"require('nan')\")" ],
    "defines"        : [ "__USE_XOPEN2K8" ],
    "conditions"   : [
      ["OS == 'mac'", {
        "xcode_settings" : {
          "MACOSX_DEPLOYMENT_TARGET" : "10.9",
          "OTHER_CFLAGS"             : [
            "-std=c++14",
            "-stdlib=libc++",
            "-Wno-unused-function",
            "-Wno-deprecated-declarations",
            "-Wno-format"
          ],
          "conditions": [
            ["target_arch=='x64'", {
              "OTHER_CFLAGS!": ["-msse4.2"]
            }],
            ["target_arch=='arm64'", {
              "OTHER_CFLAGS!": ["-march=armv8-a+crc"]
            }]
          ]
        }
      }]
    ]
  }]
}
