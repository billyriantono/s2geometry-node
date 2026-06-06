{
    "targets": [
        {
            "target_name": "NativeExtension",
            "sources": [
                "src/NativeExtension.cc",
                "src/point.cc",
                "src/latlng.cc",
                "src/cellid.cc",
                "src/cell.cc",
                "src/latlngrect.cc",
                "src/cap.cc",
                "src/interval.cc",
                "src/angle.cc",
                "src/regioncoverer.cc",
                "src/cellunion.cc",
                "src/loop.cc",
                "src/polyline.cc",
                "src/polygon.cc",
                "./geometry/s2.cc",
                "./geometry/s1interval.cc",
                "./geometry/util/math/exactfloat/exactfloat.cc",
                "./geometry/base/logging.cc",
                "./geometry/strings/split.cc",
                "./geometry/strings/strutil.cc",
                "./geometry/strings/strtoint.cc",
                "./geometry/strings/stringprintf.cc",
          	    "./geometry/s2cap.cc",
          	    "./geometry/s1angle.cc",
          	    "./geometry/s2cell.cc",
          	    "./geometry/s2cellunion.cc",
          	    "./geometry/s2cellid.cc",
          	    "./geometry/s2edgeindex.cc",
          	    "./geometry/s2edgeutil.cc",
          	    "./geometry/s2latlngrect.cc",
          	    "./geometry/s2loop.cc",
          	    "./geometry/s2pointregion.cc",
          	    "./geometry/s2latlng.cc",
          	    "./geometry/s2polygon.cc",
          	    "./geometry/s2polygonbuilder.cc",
          	    "./geometry/s2polyline.cc",
          	    "./geometry/s2r2rect.cc",
          	    "./geometry/s2region.cc",
          	    "./geometry/s2regioncoverer.cc",
          	    "./viewfinder/viewfinder.cc"
            ],
            "defines": [
                'NDEBUG',
                'GNU_C',
                '_GNU_SOURCE'
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "./viewfinder/",
                "./geometry/",
                "./geometry/base/",
                "./geometry/util/",
                "./geometry/util/math/",
                "./geometry/strings/"
            ],
            'conditions': [
                ['OS=="mac"', {
                    'defines': [
                        'S2_USE_EXACTFLOAT'
                    ],
                    'xcode_settings': {
                        'OTHER_CPLUSPLUSFLAGS':[
                            '-Wno-deprecated',
                            '-Wno-ignored-qualifiers',
                            '-Wno-absolute-value',
                            '-Wno-unused-result'
                        ],
                        'GCC_ENABLE_CPP_RTTI': 'YES',
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'CLANG_CXX_LANGUAGE_STANDARD': 'c++17',
                        'CLANG_CXX_LIBRARY': 'libc++',
                        'MACOSX_DEPLOYMENT_TARGET': '10.15',
                    }
                }],
                ['OS=="linux"', {
                    'cflags_cc' : [
                        '-Wno-deprecated',
                        '-Wno-ignored-qualifiers',
                        '-DARCH_K8',
                        '-DS2_USE_EXACTFLOAT',
                        '-std=c++17'
                    ],
                    'cflags_cc!': [ '-fno-exceptions', '-fno-rtti' ],
                }],
                ['OS=="win"', {
                    'defines': [
                        '_USE_MATH_DEFINES',
                        'NOMINMAX'
                    ],
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'AdditionalOptions': [ '/std:c++17', '/EHsc' ]
                        }
                    }
                }]
            ]
        }
    ]
}