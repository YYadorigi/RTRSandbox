project "glad"
	kind "StaticLib"
	language "C"
	cdialect "Default"
	systemversion "latest"
	staticruntime "On"

	objdir (binarydir .. "/tmp/" .. outputdir .. "/%{prj.name}")
	targetdir (binarydir .. "/" .. outputdir .. "/%{prj.name}")

	files {
		"include/glad/glad.h",
		"include/KHR/khrplatform.h",
		"src/glad.c",
	}

	includedirs {
		"include",
	}

	buildoptions {
		"/utf-8",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"