cmake_minimum_required(VERSION 2.6)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(NobleEngine)

include_directories (
    src
)

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")

	include_directories(
		contrib/msvc-windows-x86_64/include
	)
	link_directories(
		${PROJECT_SOURCE_DIR}/contrib/msvc-windows-x86_64/lib
	)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "../bin")

set (

	source_list
	
	src/Engine/Core/VersionInformation.h
	
	src/Engine/Useful.h
	src/Engine/Useful.cpp

	src/Engine/imgui/imconfig.h
	src/Engine/imgui/imgui.h
	src/Engine/imgui/imgui.cpp
	src/Engine/imgui/imgui_demo.cpp
	src/Engine/imgui/imgui_draw.cpp
	src/Engine/imgui/imgui_internal.h
	src/Engine/imgui/imgui_tables.cpp
	src/Engine/imgui/imgui_widgets.cpp
	src/Engine/imgui/imstb_rectpack.h
	src/Engine/imgui/imstb_textedit.h
	src/Engine/imgui/imstb_truetype.h
	src/Engine/imgui/imgui_stdlib.h
	src/Engine/imgui/imgui_stdlib.cpp

	src/Engine/imgui/imgui.h
	src/Engine/imgui/imgui.cpp
	src/Engine/imgui/backends/imgui_impl_vulkan.h
	src/Engine/imgui/backends/imgui_impl_vulkan.cpp

	src/Engine/imgui/implot.h
	src/Engine/imgui/implot.cpp
	src/Engine/imgui/implot_demo.cpp
	src/Engine/imgui/implot_internal.h
	src/Engine/imgui/implot_items.cpp

	src/Engine/imgui/backends/imgui_impl_sdl2.h
	src/Engine/imgui/backends/imgui_impl_sdl2.cpp

	src/Engine/imgui/ImGuiFileDialog.h
	src/Engine/imgui/ImGuiFileDialog.cpp
	src/Engine/imgui/ImGuiFileDialogConfig.h

	src/Engine/Core/ToolUI.h
	src/Engine/Core/ToolUI.cpp

	#lua
	
	src/Engine/Lua/lstrlib.c
    src/Engine/Lua/lundump.c
    src/Engine/Lua/lmathlib.c
    src/Engine/Lua/lopcodes.c
    src/Engine/Lua/lcorolib.c
    src/Engine/Lua/lfunc.c
    src/Engine/Lua/lstate.c
    src/Engine/Lua/lutf8lib.c
    src/Engine/Lua/lbitlib.c
    src/Engine/Lua/loslib.c
    src/Engine/Lua/lobject.c
    src/Engine/Lua/lapi.c
    src/Engine/Lua/lbaselib.c
    src/Engine/Lua/lzio.c
    src/Engine/Lua/lcode.c
    src/Engine/Lua/lparser.c
    src/Engine/Lua/llex.c
    src/Engine/Lua/liolib.c
    src/Engine/Lua/linit.c
    src/Engine/Lua/ldump.c
    src/Engine/Lua/loadlib.c
    src/Engine/Lua/ltablib.c
    src/Engine/Lua/lmem.c
    src/Engine/Lua/lgc.c
    src/Engine/Lua/ldebug.c
    src/Engine/Lua/ldblib.c
    src/Engine/Lua/ldo.c
    src/Engine/Lua/ltm.c
    src/Engine/Lua/ltable.c
    src/Engine/Lua/lctype.c
    src/Engine/Lua/lvm.c
    src/Engine/Lua/lstring.c
    src/Engine/Lua/lua.hpp
    src/Engine/Lua/lauxlib.c
    src/Engine/Lua/luac.c

	src/Engine/Lua/sol.hpp
	src/Engine/Lua/config.hpp

	src/Engine/Lua/NobleLuaState.h
	src/Engine/Lua/NobleLuaState.cpp

	#Core
	src/Engine/Core/Application.h
	src/Engine/Core/Application.cpp

	src/Engine/Core/CommandTypes.h
	src/Engine/Core/CommandTypes.cpp

	src/Engine/Core/NobleDLL.h
	src/Engine/Core/NobleDLL.cpp

	src/Engine/Core/AudioManager.h
	src/Engine/Core/AudioManager.cpp

	src/Engine/Core/PerformanceStats.h
	src/Engine/Core/PerformanceStats.cpp

	src/Engine/Core/ThreadingManager.h
	src/Engine/Core/ThreadingManager.cpp

	src/Engine/Core/Logger.h
	src/Engine/Core/Logger.cpp

	src/Engine/Core/InputManager.h
	src/Engine/Core/InputManager.cpp

	src/Engine/Core/ResourceManager.h
	src/Engine/Core/ResourceManager.cpp

	src/Engine/Core/Registry.h
	src/Engine/Core/Registry.cpp

	src/Engine/Core/SceneManager.h
	src/Engine/Core/SceneManager.cpp

	src/Engine/Core/ProjectFile.h
	src/Engine/Core/ProjectFile.cpp

	#Core Graphics

	src/Engine/Core/Graphics/Renderer.h
	src/Engine/Core/Graphics/Renderer.cpp
	
	src/Engine/Core/Graphics/VkBootstrap.h
	src/Engine/Core/Graphics/VkBootstrap.cpp
	src/Engine/Core/Graphics/VkBootstrapDispatch.h

	src/Engine/Core/Graphics/VulkanInitialisers.h
	src/Engine/Core/Graphics/VulkanInitialisers.cpp

	src/Engine/Core/Graphics/VulkanTypes.h

	src/Engine/Core/Graphics/VulkanImages.h
	src/Engine/Core/Graphics/VulkanImages.cpp

	src/Engine/Core/Graphics/VulkanDescriptors.h
	src/Engine/Core/Graphics/VulkanDescriptors.cpp

	src/Engine/Core/Graphics/VulkanPipelines.h
	src/Engine/Core/Graphics/VulkanPipelines.cpp

	#Maths

	src/Engine/Maths/Raycaster.h
	src/Engine/Maths/Raycaster.cpp

	#ECS

	src/Engine/Core/ECS/Entity.h
	src/Engine/Core/ECS/Entity.cpp

	src/Engine/Core/ECS/Component.h
	src/Engine/Core/ECS/Component.cpp

	src/Engine/Core/ECS/Renderable.hpp
	src/Engine/Core/ECS/ComponentList.hpp

	#Resource Handling
	
	src/Engine/Core/EngineResources/Resource.h
	src/Engine/Core/EngineResources/Resource.cpp

	src/Engine/Core/EngineResources/Texture.h
	src/Engine/Core/EngineResources/Texture.cpp

	src/Engine/Core/EngineResources/IniFile.h
	src/Engine/Core/EngineResources/IniFile.cpp

	src/Engine/Core/EngineResources/AudioClip.h
	src/Engine/Core/EngineResources/AudioClip.cpp

	src/Engine/Core/EngineResources/Model.h
	src/Engine/Core/EngineResources/Model.cpp

	src/Engine/Core/EngineResources/Scene.h
	src/Engine/Core/EngineResources/Scene.cpp

	src/Engine/Core/EngineResources/Script.h
	src/Engine/Core/EngineResources/Script.cpp

	src/Engine/Core/EngineResources/Pipeline.h
	src/Engine/Core/EngineResources/Pipeline.cpp

	#Components

	src/Engine/Core/EngineComponents/AudioListener.h
	src/Engine/Core/EngineComponents/AudioListener.cpp

	src/Engine/Core/EngineComponents/AudioSource.h
	src/Engine/Core/EngineComponents/AudioSource.cpp

	src/Engine/Core/EngineComponents/Camera.h
	src/Engine/Core/EngineComponents/Camera.cpp

	src/Engine/Core/EngineComponents/Light.h
	src/Engine/Core/EngineComponents/Light.cpp

	src/Engine/Core/EngineComponents/MeshRenderer.h
	src/Engine/Core/EngineComponents/MeshRenderer.cpp

	src/Engine/Core/EngineComponents/Sprite.h
	src/Engine/Core/EngineComponents/Sprite.cpp

	src/Engine/Core/EngineComponents/ScriptEmbedder.h
	src/Engine/Core/EngineComponents/ScriptEmbedder.cpp

	src/Engine/Core/EngineComponents/Transform.h
	src/Engine/Core/EngineComponents/Transform.cpp
)

add_definitions(-DSDL_MAIN_HANDLED)
add_library(Engine
	
	${source_list}
)
target_link_libraries(Engine SDL2 enet64 ws2_32 winmm fmod_vc freetype vulkan-1)
target_compile_features(Engine PRIVATE cxx_std_20)

foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

set (

	editor_source
	
	src/Editor/main.cpp
	
	src/Editor/resource.h
	src/Editor/Editor.rc

	#Interfaces

	src/Editor/Interface/MainMenuBar.h
	src/Editor/Interface/MainMenuBar.cpp

	src/Editor/Interface/MainDockWindow.h
	src/Editor/Interface/MainDockWindow.cpp

	src/Editor/Interface/EditorUI.h
	src/Editor/Interface/EditorUI.cpp

	src/Editor/Interface/DataEditorWindow.h
	src/Editor/Interface/DataEditorWindow.cpp

	src/Editor/Interface/ResourceManagerWindow.h
	src/Editor/Interface/ResourceManagerWindow.cpp

	src/Editor/Interface/Profiler.h
	src/Editor/Interface/Profiler.cpp

	src/Editor/Interface/EditorSettingsWindow.h
	src/Editor/Interface/EditorSettingsWindow.cpp

	src/Editor/Interface/ProjectDetailsWindow.h
	src/Editor/Interface/ProjectDetailsWindow.cpp

	src/Editor/Interface/SceneHierarchyWindow.h
	src/Editor/Interface/SceneHierarchyWindow.cpp

	src/Editor/Interface/SceneManagerWindow.h
	src/Editor/Interface/SceneManagerWindow.cpp

	src/Editor/Interface/SceneViewWindow.h
	src/Editor/Interface/SceneViewWindow.cpp

	#Editor management
	
	src/Editor/EditorManagement/EditorManager.h
	src/Editor/EditorManagement/EditorManager.cpp

	src/Editor/EditorManagement/CommandSystem.h
	src/Editor/EditorManagement/CommandSystem.cpp

	src/Editor/EditorManagement/EditorCam.h
	src/Editor/EditorManagement/EditorCam.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/Editor>)
add_executable(Editor

	${editor_source}

)
target_link_libraries(Editor Engine)
set_target_properties(Editor PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
set_target_properties(Editor PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
set_property(TARGET Editor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Editor")

target_compile_features(Editor PRIVATE cxx_std_20)

foreach(source IN LISTS editor_source)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/TechDemos/TechDemo1>)
add_library(NobleGame SHARED

	src/NobleGame/NobleGame.h
	src/NobleGame/NobleGame.cpp

)
target_link_libraries(NobleGame Engine)
target_compile_features(NobleGame PRIVATE cxx_std_20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_SOURCE_DIR}/TechDemos/TechDemo1>)
add_executable(TechDemo1
    src/Game/main.cpp

	src/Game/World/World.hpp
)
target_link_libraries(TechDemo1 Engine)
set_target_properties(TechDemo1 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
set_target_properties(TechDemo1 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
set_property(TARGET TechDemo1 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/TechDemos/TechDemo1")

target_compile_features(TechDemo1 PRIVATE cxx_std_20)