include "./third-party/premake/premake_customization/solution_items.lua"

workspace "DawnStar"
	architecture "x86_64"
	startproject "Example"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["entt"] = "%{wks.location}/DawnStar/third-party/entt/include"
IncludeDir["Glad"] = "%{wks.location}/DawnStar/third-party/glad/include"
IncludeDir["GLFW"] = "%{wks.location}/DawnStar/third-party/glfw/include"
IncludeDir["glm"] = "%{wks.location}/DawnStar/third-party/glm"
IncludeDir["ImGui"] = "%{wks.location}/DawnStar/third-party/imgui"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/DawnStar/third-party/msdf-atlas-gen/msdf-atlas-gen"
IncludeDir["msdfgen"] = "%{wks.location}/DawnStar/third-party/msdf-atlas-gen/msdfgen"
IncludeDir["optick"] = "%{wks.location}/DawnStar/third-party/optick/src"
IncludeDir["stb_image"] = "%{wks.location}/DawnStar/third-party/stb_image"

group "Dependencies"
	include "third-party/premake"
	include "DawnStar/third-party/glfw"
	include "DawnStar/third-party/glad"
	include "DawnStar/third-party/imgui"
	include "DawnStar/third-party/optick"
	include "DawnStar/third-party/msdf-atlas-gen"
group ""

include "DawnStar"
include "Example"
