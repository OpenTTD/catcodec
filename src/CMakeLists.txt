cmake_minimum_required(VERSION 3.16)

# Add files for catcodec
target_sources(catcodec PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/catcodec.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/io.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/io.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sample.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sample.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h
)
