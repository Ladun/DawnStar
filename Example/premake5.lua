project "Example"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.hpp",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/DawnStar/src",
		"%{wks.location}/DawnStar/third-party"
	}

	externalincludedirs
	{
		"%{wks.location}/DawnStar/third-party/spdlog/include",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.optick}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}"
	}

	links
	{
		"DawnStar",
		"GLFW",
		"Glad"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"GLM_ENABLE_EXPERIMENTAL"
		}	

	filter "configurations:Debug"
		defines "DS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "DS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "DS_DIST"
		runtime "Release"
		optimize "on"
	