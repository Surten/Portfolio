//----------------------------------------------------------------------------------------
/**
 * \file    main.cpp
 * \author  Josef Bacík
 * \date    2020
 * \brief   data to load fire texture, banner and cube
 */
 //----------------------------------------------------------------------------------------

#ifndef __LOAD_H
#define __LOAD_H

const int fireNumQuadVertices = 4;
const float fireVertexData[fireNumQuadVertices * 5] = {
	// x      y     z     u     v
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
};

const int bannerNumQuadVertices = 4;
const float bannerVertexData[bannerNumQuadVertices * 5] = {

	// x      y      z     u     v
	-1.5f,  0.15f, 0.0f, 0.0f, 1.0f,
	-1.5f, -0.15f, 0.0f, 0.0f, 0.0f,
	 1.5f,  0.15f, 0.0f, 3.0f, 1.0f,
	 1.5f, -0.15f, 0.0f, 3.0f, 0.0f
};

const int cubeTrianglesCount = 12;
const float cubeVertices[] = {
	// x    y     z      r     g     b     nx     ny     nz
	-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	
};
const unsigned int cubeIndices[]{
 0,1,2,
 1,2,3,
 1,3,5,
 3,5,7,
 0,1,5,
 0,4,5,
 4,6,7,
 4,5,7,
 2,3,7,
 2,6,7,
 0,2,6,
 0,4,6,
};

#endif
