workspace "Jaci"
	configurations {"Debug", "Release"}
	toolset "gcc"

filter "configurations:Debug"
	defines { "DEBUG" }
	symbols "On"

filter "configurations:Release"
	defines { "NDEBUG" }
	optimize "On"

-- global filter
filter {}
	buildoptions { "-Wall", "-Wextra" }
	defines { "FUNCHOOK_USE_DISTORM", "DISASM_DISTORM" }

---

project "Jaci"
	kind "SharedLib"
	language "C++"
	cppdialect "C++23"
	cdialect "C23"
	links { "SDL2", "vulkan" }
	files {
		"src/**.cpp",

		"thirdparty/imgui/*.cpp",

		"thirdparty/libsigscan/libsigscan.c",
		"thirdparty/funchook/funchook.c",
		"thirdparty/funchook/funchook_unix.c",
		"thirdparty/funchook/funchook_x86.c",
		"thirdparty/funchook/disasm_distorm.c",
		"thirdparty/distorm/decoder.c",
		"thirdparty/distorm/distorm.c",
		"thirdparty/distorm/instructions.c",
		"thirdparty/distorm/insts.c",
		"thirdparty/distorm/mnemonics.c",
		"thirdparty/distorm/operands.c",
		"thirdparty/distorm/prefix.c",
		"thirdparty/distorm/textdefs.c",
	}

	filter { "configurations:Debug" }
	targetdir "build/debug"

	filter { "configurations:Release" }
	targetdir "build/release"

	filter { "files:thirdparty/imgui/*.cpp" }
	removebuildoptions { "-Wall", "-Wextra" } -- remove global build options as I can't control thirparty libraries to follow -Wall and -Wextra

	filter {} -- reset filter
---