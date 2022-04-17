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
    os.remove("owlcore.vcxproj")
    os.remove("owlcore.vcxproj.filters")
    os.remove("owlcore.vcxproj.user")
    os.remove("owlvm.vcxproj")
    os.remove("owlvm.vcxproj.filters")
    os.remove("owlvm.vcxproj.user")
    os.remove("owl.vcxproj")
    os.remove("owl.vcxproj.filters")
    os.remove("owl.vcxproj.user")
    os.remove("owlcore.make")
    os.remove("owlvm.make")
    os.remove("owl.make")
    os.remove("Makefile")
    return
  end

  -- A project defines one build target
  project ( "owlcore" )
    kind ( "SharedLib" )
    language ( "C" )
    files { "./core/**.h", "./core/**.c",
            "./include/owl.h", "./3rd/*.h",
            "./3rd/utf8/utf8.h", "./3rd/utf8/utf8.c" }
    includedirs { "./include", "./3rd",
                  "./3rd/sdl2/include",
                  "./3rd/utf8" }
    libdirs { "./libs" }
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

    filter { "action:vs*", "configurations:Release" }
      libdirs { "./libs/sdl2/Release" }
      postbuildcommands {
        "{COPY} ./libs/sdl2/Release/SDL2.dll %{cfg.targetdir}",
        "{COPY} ./libs/sdl2/Release/SDL2.pdb %{cfg.targetdir}"
      }

    filter { "action:vs*", "configurations:Debug" }
      libdirs { "./libs/sdl2/Debug" }
      postbuildcommands {
        "{COPY} ./libs/sdl2/Debug/SDL2.dll %{cfg.targetdir}",
        "{COPY} ./libs/sdl2/Debug/SDL2.pdb %{cfg.targetdir}"
      }

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"
      links { "m", "iconv" }

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }
      linkoptions { "-rpath @executable_path" }
      links { "CoreFoundation.framework" }
      postbuildcommands {
        "{COPY} ./libs/libSDL2.dylib %{cfg.targetdir}",
      }

    filter { "action:gmake", "system:linux" }
      defines { "__linux__" }
      linkoptions { "-Wl,-rpath=." }

    filter { "action:gmake", "system:bsd" }
      defines { "__BSD__" }


  -- A project defines one build target
  project ( "owlvm" )
    kind ( "SharedLib" )
    language ( "C" )
    files { "./vm/**.h", "./vm/**.c",
            "./include/owlvm.h" }
    includedirs { "./include" }
    libdirs { "./libs" }
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

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }
      linkoptions { "-rpath @executable_path" }

    filter { "action:gmake", "system:linux" }
      defines { "__linux__" }
      linkoptions { "-Wl,-rpath=." }

    filter { "action:gmake", "system:bsd" }
      defines { "__BSD__" }


  -- A project defines one build target
  project ( "owl" )
    kind ( "WindowedApp" )
    language ( "C" )
    files { "./src/**.h", "./src/**.c" }
    includedirs { "./include", "./3rd/sdl2/include" }
    libdirs { "./libs", "./bin" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    links { "SDL2main", "OwlCore", "OwlVM" }
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
      links { "SDL2" }

    filter { "action:vs*", "configurations:Release" }
      libdirs { "./libs/sdl2/Release" }

    filter { "action:vs*", "configurations:Debug" }
      libdirs { "./libs/sdl2/Debug" }

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"
      links { "m", "iconv" }

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }

    filter { "action:gmake", "system:linux" }
      defines { "__linux__" }

    filter { "action:gmake", "system:bsd" }
      defines { "__BSD__" }
