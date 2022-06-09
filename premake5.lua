workspace "tiger"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "tiger"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "tiger"
    location "tiger"
    kind "ConsoleApp"
    language "C"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/tiger.lex",
        "%{prj.name}/src/tiger.grm"
    }

    filter "system:windows"
        prebuildcommands
        {
            "set M4=%{wks.location}/vendor/bison/bin/m4.exe",
            "start /b %{wks.location}/vendor/bison/bin/bison.exe -o%{wks.location}/%{prj.name}/src/y.tab.c -y -dv %{wks.location}/%{prj.name}/src/tiger.grm",
            "start /b %{wks.location}/vendor/flex/bin/flex.exe -o%{wks.location}/%{prj.name}/src/lex.yy.c %{wks.location}/%{prj.name}/src/tiger.lex",
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "On"