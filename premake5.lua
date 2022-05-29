workspace "tiger"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "tiger"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "tiger"
    kind "ConsoleApp"
    language "C"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "On"