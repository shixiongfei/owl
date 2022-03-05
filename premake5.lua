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
  platforms { "x64" }

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
    kind ( "WindowedApp" )
    language ( "C" )
    files { "./src/**.h", "./src/**.c" }
    includedirs { "./3rd/sdl2/include" }
    libdirs { "./libs" }
    objdir ( "./objs" )
    targetdir ( "./bin" )
    links { "SDL2", "SDL2main" }
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

    filter ( "action:gmake" )
      warnings  "Default" --"Extra"

    filter { "action:gmake", "system:macosx" }
      defines { "__APPLE__", "__MACH__", "__MRC__", "macintosh" }

    filter { "action:gmake", "system:linux" }
      defines { "__linux__" }

    filter { "action:gmake", "system:bsd" }
      defines { "__BSD__" }
