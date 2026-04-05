{
  "targets": [{
    "target_name"    : "metrohash",
    "sources"        : [ "./src/metrohash_wrapper.cpp", "./src/metrohash128.cpp", "./src/metrohash128crc.cpp", "./src/metrohash64.cpp" ],
    "cflags"         : [
      "<!(node -e \"console.log(parseInt(process.versions.node)>=24?'-std=c++20':'-std=c++17')\")",
      "-Wno-deprecated-declarations",
      "-Wno-unused-value",
      "-Wno-unused-function",
      "-Wno-unknown-pragmas",
      "-Wno-format",
      "-msse4.2"
    ],
    "include_dirs"   : [ "<!(node -e \"require('nan')\")" ],
    "defines"        : [ "__USE_XOPEN2K8" ],
    "conditions"   : [
      ["OS == 'linux' and target_arch == 'arm64'", {
        "cflags!": ["-msse4.2"],
        "cflags": ["-march=armv8-a+crc"]
      }],
      ["OS == 'mac'", {
        "xcode_settings" : {
          "MACOSX_DEPLOYMENT_TARGET" : "10.9",
          "OTHER_CFLAGS"             : [
            "<!(node -e \"console.log(parseInt(process.versions.node)>=24?'-std=c++20':'-std=c++17')\")",
            "-stdlib=libc++",
            "-Wno-unused-function",
            "-Wno-deprecated-declarations",
            "-Wno-format",
            "-msse4.2"
          ],
          "conditions": [
            ["target_arch=='arm64'", {
              "OTHER_CFLAGS": ["-march=armv8-a+crc"],
              "OTHER_CFLAGS!": ["-msse4.2"]
            }]
          ]
        }
      }]
    ]
  }]
}
