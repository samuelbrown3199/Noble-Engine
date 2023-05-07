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

add_library(Engine

	src/Engine/Useful.h
	src/Engine/Useful.cpp

	#Networking
	src/Engine/Networking/NetworkManager.h
	src/Engine/Networking/NetworkManager.cpp

	src/Engine/Networking/Packet.hpp
	src/Engine/Networking/PacketParser.h

	#System

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
)
target_link_libraries(Engine SDL2 OpenAL32 glew32 opengl32 enet64 ws2_32 winmm freetype)

add_executable(Editor
	src/Editor/main.cpp
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