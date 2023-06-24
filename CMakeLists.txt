cmake_minimum_required(VERSION 2.6)
project(NobleEngine)

include_directories(
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

add_definitions(-DSDL_MAIN_HANDLED)

add_library(Engine ${SRC_IMGUI}

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

	src/Engine/imgui/backends/imgui_impl_opengl3.h
	src/Engine/imgui/backends/imgui_impl_opengl3.cpp

	src/Engine/imgui/backends/imgui_impl_sdl2.h
	src/Engine/imgui/backends/imgui_impl_sdl2.cpp

	src/Engine/Core/DebugUI.hpp

	#Networking
	src/Engine/Networking/NetworkManager.h
	src/Engine/Networking/NetworkManager.cpp

	src/Engine/Networking/Packet.hpp
	src/Engine/Networking/PacketParser.h

	#Behaviours
	src/Engine/Behaviours/DebugCam.h
	src/Engine/Behaviours/DebugCam.cpp

	#Core
	src/Engine/Core/Application.h
	src/Engine/Core/Application.cpp

	src/Engine/Core/AudioManager.h
	src/Engine/Core/AudioManager.cpp

	src/Engine/Core/PerformanceStats.h
	src/Engine/Core/PerformanceStats.cpp

	src/Engine/Core/ThreadingManager.h
	src/Engine/Core/ThreadingManager.cpp

	src/Engine/Core/Logger.h
	src/Engine/Core/Logger.cpp

	src/Engine/Core/Renderer.h
	src/Engine/Core/Renderer.cpp

	src/Engine/Core/InputManager.h
	src/Engine/Core/InputManager.cpp

	src/Engine/Core/ResourceManager.h
	src/Engine/Core/ResourceManager.cpp

	src/Engine/Core/SceneManager.h
	src/Engine/Core/SceneManager.cpp

	src/Engine/Core/UI.h
	src/Engine/Core/UI.cpp

	#ECS

	src/Engine/ECS/Entity.hpp
	src/Engine/ECS/Component.hpp
	src/Engine/ECS/System.hpp
	src/Engine/ECS/Behaviour.hpp

	#Resource Handling
	
	src/Engine/Resource/Resource.h

	src/Engine/Resource/ShaderProgram.hpp

	src/Engine/Resource/Texture.h
	src/Engine/Resource/Texture.cpp

	src/Engine/Resource/IniFile.h
	src/Engine/Resource/IniFile.cpp

	src/Engine/Resource/AudioClip.h
	src/Engine/Resource/AudioClip.cpp

	src/Engine/Resource/Font.h
	src/Engine/Resource/Font.cpp

	src/Engine/Resource/UILayout.h
	src/Engine/Resource/UILayout.cpp

	src/Engine/Resource/ModelLoader.hpp
	src/Engine/Resource/Model.h
	src/Engine/Resource/Model.cpp

	src/Engine/Resource/Scene.h
	src/Engine/Resource/Scene.cpp

	#Systems & components

	src/Engine/Systems/AudioSource.h
	src/Engine/Systems/AudioSourceSystem.h
	src/Engine/Systems/AudioSourceSystem.cpp

	src/Engine/Systems/Transform.h
	src/Engine/Systems/TransformSystem.h
	src/Engine/Systems/TransformSystem.cpp

	src/Engine/Systems/Sprite.h
	src/Engine/Systems/SpriteSystem.h
	src/Engine/Systems/SpriteSystem.cpp

	src/Engine/Systems/MeshRenderer.h
	src/Engine/Systems/MeshRendererSystem.h
	src/Engine/Systems/MeshRendererSystem.cpp
)
target_link_libraries(Engine SDL2 OpenAL32 glew32 opengl32 enet64 ws2_32 winmm freetype)

add_executable(Editor
	src/Editor/main.cpp

	src/Editor/EditorUI.hpp
)
target_link_libraries(Editor Engine)

add_executable(TechDemo1
    src/Game/main.cpp

	src/Game/Interface/MainMenu.h
	src/Game/Interface/MainMenu.cpp

	src/Game/World/World.hpp
	
	src/Game/World/TileChunk.h
	src/Game/World/TileChunkSystem.h
	src/Game/World/TileChunkSystem.cpp
)
target_link_libraries(TechDemo1 Engine)
set_target_properties(TechDemo1 PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
set_target_properties(TechDemo1 PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")