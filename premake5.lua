-- premake5.lua
--
-- Copyright (c) 2022 Xiongfei Shi. All rights reserved.
--
-- Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
--
-- This file is part of Owl.
-- Usage of Owl is subject to the appropriate license agreement.
--

workspace ( "owl" )
  configurations { "Release", "Debug" }
  startproject "owl"

  if os.istarget("macosx") or os.istarget("ios") then
    platforms { "universal" }
  else
    platforms { "x64" }
  end


  if _ACTION == "clean" then
    os.rmdir(".vs")
    os.rmdir("bin")
    os.rmdir("objs")
    os.remove("owl.VC.db")
    os.remove("owl.sln")
    os.remove("SDL2.vcxproj")
    os.remove("SDL2.vcxproj.filters")
    os.remove("SDL2.vcxproj.user")
    os.remove("SDL2main.vcxproj")
    os.remove("SDL2main.vcxproj.filters")
    os.remove("SDL2main.vcxproj.user")
    os.remove("owlcore.vcxproj")
    os.remove("owlcore.vcxproj.filters")
    os.remove("owlcore.vcxproj.user")
    os.remove("owlvm.vcxproj")
    os.remove("owlvm.vcxproj.filters")
    os.remove("owlvm.vcxproj.user")
    os.remove("owl.vcxproj")
    os.remove("owl.vcxproj.filters")
    os.remove("owl.vcxproj.user")
    os.remove("SDL2.make")
    os.remove("SDL2main.make")
    os.remove("owlcore.make")
    os.remove("owlvm.make")
    os.remove("owl.make")
    os.remove("Makefile")
    return
  end


  -- A project defines one build target
  project ( "SDL2" )
    kind ( "SharedLib" )
    language ( "C" )
    files { "./3rd/sdl2/include/*.h",
            "./3rd/sdl2/src/*.h", "./3rd/sdl2/src/*.c",
            "./3rd/sdl2/src/atomic/*.h", "./3rd/sdl2/src/atomic/*.c",
            "./3rd/sdl2/src/audio/*.h", "./3rd/sdl2/src/audio/*.c",
            "./3rd/sdl2/src/audio/dummy/*.h", "./3rd/sdl2/src/audio/dummy/*.c",
            "./3rd/sdl2/src/cpuinfo/*.h", "./3rd/sdl2/src/cpuinfo/*.c",
            "./3rd/sdl2/src/dynapi/*.h", "./3rd/sdl2/src/dynapi/*.c",
            "./3rd/sdl2/src/events/*.h", "./3rd/sdl2/src/events/*.c",
            "./3rd/sdl2/src/file/*.h", "./3rd/sdl2/src/file/*.c",
            "./3rd/sdl2/src/filesystem/*.h", "./3rd/sdl2/src/filesystem/*.c",
            "./3rd/sdl2/src/haptic/*.h", "./3rd/sdl2/src/haptic/*.c",
            "./3rd/sdl2/src/haptic/dummy/*.h", "./3rd/sdl2/src/haptic/dummy/*.c",
            "./3rd/sdl2/src/hidapi/*.h", "./3rd/sdl2/src/hidapi/*.c",
            "./3rd/sdl2/src/joystick/*.h", "./3rd/sdl2/src/joystick/*.c",
            "./3rd/sdl2/src/joystick/dummy/*.h", "./3rd/sdl2/src/joystick/dummy/*.c",
            "./3rd/sdl2/src/joystick/hidapi/*.h", "./3rd/sdl2/src/joystick/hidapi/*.c",
            "./3rd/sdl2/src/joystick/virtual/*.h", "./3rd/sdl2/src/joystick/virtual/*.c",
            "./3rd/sdl2/src/libm/*.h", "./3rd/sdl2/src/libm/*.c",
            "./3rd/sdl2/src/loadso/*.h", "./3rd/sdl2/src/loadso/*.c",
            "./3rd/sdl2/src/locale/*.h", "./3rd/sdl2/src/locale/*.c",
            "./3rd/sdl2/src/misc/*.h", "./3rd/sdl2/src/misc/*.c",
            "./3rd/sdl2/src/power/*.h", "./3rd/sdl2/src/power/*.c",
            "./3rd/sdl2/src/render/*.h", "./3rd/sdl2/src/render/*.c",
            "./3rd/sdl2/src/render/opengl/*.h", "./3rd/sdl2/src/render/opengl/*.c",
            "./3rd/sdl2/src/render/opengles2/*.h", "./3rd/sdl2/src/render/opengles2/*.c",
            "./3rd/sdl2/src/render/software/*.h", "./3rd/sdl2/src/render/software/*.c",
            "./3rd/sdl2/src/sensor/*.h", "./3rd/sdl2/src/sensor/*.c",
            "./3rd/sdl2/src/sensor/dummy/*.h", "./3rd/sdl2/src/sensor/dummy/*.c",
            "./3rd/sdl2/src/stdlib/*.h", "./3rd/sdl2/src/stdlib/*.c",
            "./3rd/sdl2/src/thread/*.h", "./3rd/sdl2/src/thread/*.c",
            "./3rd/sdl2/src/timer/*.h", "./3rd/sdl2/src/timer/*.c",
            "./3rd/sdl2/src/video/*.h", "./3rd/sdl2/src/video/*.c",
            "./3rd/sdl2/src/video/dummy/*.h", "./3rd/sdl2/src/video/dummy/*.c",
            "./3rd/sdl2/src/video/khronos/*.h", "./3rd/sdl2/src/video/khronos/*.c",
            "./3rd/sdl2/src/video/yuv2rgb/*.h", "./3rd/sdl2/src/video/yuv2rgb/*.c" }
    includedirs { "./3rd/sdl2/include" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    defines { "_UNICODE", "DLL_EXPORT" }
    staticruntime "On"

    filter ( "configurations:Release" )
      optimize "On"
      defines { "NDEBUG", "_NDEBUG" }

    filter ( "configurations:Debug" )
      symbols "On"
      defines { "DEBUG", "_DEBUG" }

    filter ( "action:vs*" )
      defines { "WIN32", "_WIN32", "_WINDOWS",
                "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE",
                "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
      files { "./3rd/sdl2/src/audio/directsound/*.h", "./3rd/sdl2/src/audio/directsound/*.c",
              "./3rd/sdl2/src/audio/disk/*.h", "./3rd/sdl2/src/audio/disk/*.c",
              "./3rd/sdl2/src/audio/wasapi/*.h", "./3rd/sdl2/src/audio/wasapi/*.c",
              "./3rd/sdl2/src/audio/winmm/*.h", "./3rd/sdl2/src/audio/winmm/*.c",
              "./3rd/sdl2/src/core/windows/*.h", "./3rd/sdl2/src/core/windows/*.c",
              "./3rd/sdl2/src/filesystem/windows/*.h", "./3rd/sdl2/src/filesystem/windows/*.c",
              "./3rd/sdl2/src/haptic/windows/*.h", "./3rd/sdl2/src/haptic/windows/*.c",
              "./3rd/sdl2/src/joystick/windows/*.h", "./3rd/sdl2/src/joystick/windows/*.c",
              "./3rd/sdl2/src/loadso/windows/*.h", "./3rd/sdl2/src/loadso/windows/*.c",
              "./3rd/sdl2/src/locale/windows/*.h", "./3rd/sdl2/src/locale/windows/*.c",
              "./3rd/sdl2/src/misc/windows/*.h", "./3rd/sdl2/src/misc/windows/*.c",
              "./3rd/sdl2/src/power/windows/*.h", "./3rd/sdl2/src/power/windows/*.c",
              "./3rd/sdl2/src/render/direct3d/*.h", "./3rd/sdl2/src/render/direct3d/*.c",
              "./3rd/sdl2/src/render/direct3d11/*.h", "./3rd/sdl2/src/render/direct3d11/*.c",
              "./3rd/sdl2/src/sensor/windows/*.h", "./3rd/sdl2/src/sensor/windows/*.c",
              "./3rd/sdl2/src/thread/generic/SDL_syscond_c.h", "./3rd/sdl2/src/thread/generic/SDL_syscond.c",
              "./3rd/sdl2/src/thread/windows/*.h", "./3rd/sdl2/src/thread/windows/*.c",
              "./3rd/sdl2/src/timer/windows/*.h", "./3rd/sdl2/src/timer/windows/*.c",
              "./3rd/sdl2/src/video/windows/*.h", "./3rd/sdl2/src/video/windows/*.c" }
      flags { "NoRuntimeChecks", "NoBufferSecurityCheck" }
      links { "setupapi.lib", "winmm.lib", "imm32.lib", "version.lib" }

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"
      files { "./3rd/sdl2/src/loadso/dummy/*.h", "./3rd/sdl2/src/loadso/dummy/*.c",
              "./3rd/sdl2/src/loadso/dlopen/*.h", "./3rd/sdl2/src/loadso/dlopen/*.c",
              "./3rd/sdl2/src/thread/pthread/*.h", "./3rd/sdl2/src/thread/pthread/*.c",
              "./3rd/sdl2/src/timer/dummy/*.h", "./3rd/sdl2/src/timer/dummy/*.c",
              "./3rd/sdl2/src/timer/unix/*.h", "./3rd/sdl2/src/timer/unix/*.c", }
      linkoptions { "-rpath @executable_path", "-rpath @loader_path" }

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh", "MAC_OS_X_VERSION_MIN_REQUIRED=1060",
                "TARGET_API_MAC_CARBON", "TARGET_API_MAC_OSX", "_THREAD_SAFE" }
      includedirs { "./3rd/sdl2/src/video/khronos", "./3rd/sdl2/src/hidapi/hidapi" }
      files { "./3rd/sdl2/src/audio/coreaudio/*.h", "./3rd/sdl2/src/audio/coreaudio/*.m",
              "./3rd/sdl2/src/audio/disk/*.h", "./3rd/sdl2/src/audio/disk/*.c",
              "./3rd/sdl2/src/file/cocoa/*.h", "./3rd/sdl2/src/file/cocoa/*.m",
              "./3rd/sdl2/src/filesystem/cocoa/*.h", "./3rd/sdl2/src/filesystem/cocoa/*.m",
              "./3rd/sdl2/src/filesystem/dummy/*.h", "./3rd/sdl2/src/filesystem/dummy/*.c",
              "./3rd/sdl2/src/haptic/darwin/*.h", "./3rd/sdl2/src/haptic/darwin/*.c",
              "./3rd/sdl2/src/hidapi/mac/*.h", "./3rd/sdl2/src/hidapi/mac/*.c",
              "./3rd/sdl2/src/joystick/darwin/*.h", "./3rd/sdl2/src/joystick/darwin/*.c",
              "./3rd/sdl2/src/joystick/iphoneos/*.h", "./3rd/sdl2/src/joystick/iphoneos/*.m",
              "./3rd/sdl2/src/joystick/steam/*.h", "./3rd/sdl2/src/joystick/steam/*.c",
              "./3rd/sdl2/src/locale/macosx/*.h", "./3rd/sdl2/src/locale/macosx/*.m",
              "./3rd/sdl2/src/misc/macosx/*.h", "./3rd/sdl2/src/misc/macosx/*.m",
              "./3rd/sdl2/src/power/macosx/*.h", "./3rd/sdl2/src/power/macosx/*.c",
              "./3rd/sdl2/src/power/uikit/*.h", "./3rd/sdl2/src/power/uikit/*.m",
              "./3rd/sdl2/src/render/metal/*.h", "./3rd/sdl2/src/render/metal/*.m", "./3rd/sdl2/src/render/metal/*.metal",
              "./3rd/sdl2/src/render/opengles/*.h", "./3rd/sdl2/src/render/opengles/*.c",
              "./3rd/sdl2/src/sensor/coremotion/*.h", "./3rd/sdl2/src/sensor/coremotion/*.m",
              "./3rd/sdl2/src/video/cocoa/*.h", "./3rd/sdl2/src/video/cocoa/*.m",
              "./3rd/sdl2/src/video/offscreen/*.h", "./3rd/sdl2/src/video/offscreen/*.c",
              "./3rd/sdl2/src/video/uikit/*.h", "./3rd/sdl2/src/video/uikit/*.m" }
      buildoptions { "-mmacosx-version-min=10.6", "-Wno-deprecated-declarations" }
      linkoptions { "-Wl,-weak_framework,CoreHaptics", "-Wl,-weak_framework,GameController",
                    "-Wl,-weak_framework,QuartzCore", "-Wl,-weak_framework,Metal" }
      links { "AudioToolbox.framework", "Carbon.framework", "Cocoa.framework", "CoreAudio.framework",
              "CoreFoundation.framework", "CoreVideo.framework", "ForceFeedback.framework", "IOKit.framework" }

    -- TODO: IOS make
    filter { "action:gmake", "system:ios" }
      defines { "__APPLE__", "__IOS__" }
      includedirs { "./3rd/sdl2/src/video/khronos", "./3rd/sdl2/src/hidapi/hidapi" }
      files { "./3rd/sdl2/src/hidapi/ios/*.h", "./3rd/sdl2/src/hidapi/ios/*.m",
              "./3rd/sdl2/src/main/uikit/*.h", "./3rd/sdl2/src/main/uikit/*.c",
              "./3rd/sdl2/src/misc/ios/*.h", "./3rd/sdl2/src/misc/ios/*.m" }
      buildoptions {}
      linkoptions {}
      links {}

  -- A project defines one build target
  project ( "SDL2main" )
    kind ( "StaticLib" )
    language ( "C" )
    files { "./3rd/sdl2/include/SDL_main.h" }
    includedirs { "./3rd/sdl2/include" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    defines { "_UNICODE" }
    staticruntime "On"

    filter ( "configurations:Release" )
      optimize "On"
      defines { "NDEBUG", "_NDEBUG" }

    filter ( "configurations:Debug" )
      symbols "On"
      defines { "DEBUG", "_DEBUG" }

    filter ( "action:vs*" )
      defines { "WIN32", "_WIN32", "_WINDOWS",
                "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE",
                "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
      files { "./3rd/sdl2/src/main/windows/*.h",
              "./3rd/sdl2/src/main/windows/*.c" }

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"
      files { "./3rd/sdl2/src/main/dummy/*.h",
              "./3rd/sdl2/src/main/dummy/*.c" }

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }


  -- A project defines one build target
  project ( "owlcore" )
    kind ( "SharedLib" )
    language ( "C" )
    files { "./core/**.h", "./core/**.c",
            "./include/owl.h", "./3rd/*.h",
            "./3rd/utf8/utf8.h", "./3rd/utf8/utf8.c",
            "./3rd/miniz/*.h", "./3rd/miniz/*.c" }
    includedirs { "./include", "./3rd",
                  "./3rd/sdl2/include",
                  "./3rd/utf8" }
    libdirs { "./bin" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    targetname ( "OwlCore" )
    links { "SDL2" }
    defines { "_UNICODE", "OWL_BUILD_DLL" }
    staticruntime "On"

    filter ( "configurations:Release" )
      optimize "On"
      defines { "NDEBUG", "_NDEBUG" }

    filter ( "configurations:Debug" )
      symbols "On"
      defines { "DEBUG", "_DEBUG" }

    filter ( "action:vs*" )
      defines { "WIN32", "_WIN32", "_WINDOWS",
                "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE",
                "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"
      linkoptions { "-rpath @executable_path", "-rpath @loader_path" }
      links { "m", "iconv" }

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }
      links { "CoreFoundation.framework" }


  -- A project defines one build target
  project ( "owlvm" )
    kind ( "SharedLib" )
    language ( "C" )
    files { "./vm/**.h", "./vm/**.c",
            "./include/owlvm.h",
            "./3rd/libbf/cutils.h", "./3rd/libbf/cutils.c",
            "./3rd/libbf/libbf.h", "./3rd/libbf/libbf.c" }
    includedirs { "./include", "./3rd/libbf" }
    libdirs { "./bin" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    targetname ( "OwlVM" )
    defines { "_UNICODE", "OWL_BUILD_DLL" }
    staticruntime "On"

    filter ( "configurations:Release" )
      optimize "On"
      defines { "NDEBUG", "_NDEBUG" }

    filter ( "configurations:Debug" )
      symbols "On"
      defines { "DEBUG", "_DEBUG" }

    filter ( "action:vs*" )
      defines { "WIN32", "_WIN32", "_WINDOWS",
                "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE",
                "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"
      linkoptions { "-rpath @executable_path", "-rpath @loader_path" }

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }


  -- A project defines one build target
  project ( "owl" )
    kind ( "WindowedApp" )
    language ( "C" )
    files { "./src/**.h", "./src/**.c",
            "./3rd/actor/*.h", "./3rd/actor/*.c",
            "./3rd/nio4c/*.h", "./3rd/nio4c/*.c",
            "./3rd/chipmunk2d/include/**.h", "./3rd/chipmunk2d/src/*.c" }
    excludes { "./3rd/actor/test.c", "./3rd/nio4c/test.c" }
    includedirs { "./include", "./3rd/sdl2/include",
                  "./3rd/actor", "./3rd/nio4c",
                  "./3rd/chipmunk2d/include" }
    libdirs { "./bin" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    links { "SDL2main", "OwlCore", "OwlVM" }
    defines { "_UNICODE", "ACTOR_STATIC", "NIO_STATIC" }
    staticruntime "On"

    filter ( "configurations:Release" )
      optimize "On"
      defines { "NDEBUG", "_NDEBUG" }

    filter ( "configurations:Debug" )
      symbols "On"
      defines { "DEBUG", "_DEBUG" }

    filter ( "action:vs*" )
      defines { "WIN32", "_WIN32", "_WINDOWS",
                "_CRT_SECURE_NO_WARNINGS", "_CRT_SECURE_NO_DEPRECATE",
                "_CRT_NONSTDC_NO_DEPRECATE", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
      links { "Ws2_32", "IPHLPAPI", "SDL2" }

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }
      links { "Foundation.framework", "IOKit.framework" }
