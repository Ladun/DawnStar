project "DawnStar"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "dspch.hpp"
	pchsource "src/dspch.cpp"

	files
	{
		"src/**.hpp",
		"src/**.cpp",
		"third-party/stb_image/**.h",
		"third-party/stb_image/**.cpp",
		"third-party/glm/glm/**.hpp",
		"third-party/glm/glm/**.inl",
		"third-party/tinyobj/**.hpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"src"
	}

	externalincludedirs
	{
		"third-party/spdlog/include",
		"third-party/tinyobj",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.msdfgen}",
		"%{IncludeDir.msdf_atlas_gen}",
		"%{IncludeDir.optick}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"optick",
		"msdf-atlas-gen",
		"opengl32.lib"
	}

	flags { "NoPCH" }

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

