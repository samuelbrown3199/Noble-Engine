cmake_minimum_required(VERSION 2.6)
project(Noble)

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
add_library(NobleEngine

#Noble Core
	src/NobleEngine/Core/Application.h
	src/NobleEngine/Core/Application.cpp
	
	src/NobleEngine/Core/Screen.h
	src/NobleEngine/Core/Screen.cpp

	src/NobleEngine/Core/Renderer.hpp

	src/NobleEngine/Core/AudioManager.h
	src/NobleEngine/Core/AudioManager.cpp

	src/NobleEngine/Core/InputManager.h
	src/NobleEngine/Core/InputManager.cpp

#Noble ECS

	src/NobleEngine/ECS/Entity.hpp
	src/NobleEngine/ECS/Component.hpp
	src/NobleEngine/ECS/System.hpp

#Noble Components

	src/NobleEngine/Components/StaticTransform.hpp
	src/NobleEngine/Systems/StaticTransformSystem.hpp

	src/NobleEngine/Components/Transform.hpp
	src/NobleEngine/Systems/TransformSystem.hpp
)
target_link_libraries(NobleEngine SDL2 openal32 glew32 opengl32)

find_package(Vulkan)
if(Vulkan_FOUND)
	target_include_directories(NobleEngine PRIVATE Vulkan::Vulkan)
	target_link_libraries(NobleEngine Vulkan::Vulkan)
endif()

add_executable(TestProgram
	src/TestProgram/main.cpp
)
target_link_libraries(TestProgram NobleEngine)