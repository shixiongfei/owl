-- premake5.lua
--
-- Copyright (c) 2022 Xiongfei Shi. All rights reserved.
--
-- Author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
--
-- This file is part of P8.
-- Usage of P8 is subject to the appropriate license agreement.
--

workspace ( "p8" )
  configurations { "Release", "Debug" }
  platforms { "x64", "x32" }

  if _ACTION == "clean" then
    os.rmdir(".vs")
    os.rmdir("bin")
    os.rmdir("objs")
    os.remove("p8.VC.db")
    os.remove("p8.sln")
    os.remove("p8.vcxproj")
    os.remove("p8.vcxproj.filters")
    os.remove("p8.vcxproj.user")
    os.remove("p8.make")
    os.remove("Makefile")
    return
  end

  -- A project defines one build target
  project ( "p8" )
    kind ( "SharedLib" )
    language ( "C" )
    files { "./include/**.h", "./src/**.h", "./src/**.c" }
    includedirs { "./include" }
    libdirs { }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    defines { "_UNICODE","P8_BUILD_DLL" }
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

    filter { "action:gmake", "system:linux" }
      defines { "__linux__" }

    filter { "action:gmake", "system:bsd" }
      defines { "__BSD__" }


  -- A project defines one build target
  project ( "p8-test" )
    kind ( "ConsoleApp" )
    language ( "C" )
    files { "./test/*.c" }
    includedirs { "./include" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    defines { "_UNICODE" }
    links { "p8" }
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

    filter { "action:gmake", "system:linux" }
      defines { "__linux__" }

    filter { "action:gmake", "system:bsd" }
      defines { "__BSD__" }
