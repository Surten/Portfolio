//----------------------------------------------------------------------------------------
/**
 * \file    main.cpp
 * \author  Josef Bacík
 * \date    2020
 * \brief   responsible for communication with GPU, init objects and shaders and draws objects
 */
 //----------------------------------------------------------------------------------------


#include <iostream>
#include "pgr.h"
#include "render.h"
#include "load.h"
#include <iostream>
#include <fstream>
#include <iostream>

MeshGeometry* bannerGeometry = NULL;
MeshGeometry* fireMeshGeometry = NULL;
MeshGeometry* skyboxGeometry = NULL;
MeshGeometry* islandGeometry = NULL;
MeshGeometry* waterGeometry = NULL;
std::vector<MeshGeometry*> fireplaceGeometry;
std::vector<MeshGeometry*> heliGeometry;
std::vector<MeshGeometry*> palmGeometry;
MeshGeometry* cubeGeometry = NULL;
std::vector<MeshGeometry*> rockGeometry;
std::vector<MeshGeometry*> benchGeometry;

const char* BANNER_TEXTURE = "data/banner.png";
const char* SKYBOX_PREFIX = "data/skybox/skybox";
const char* ISLAND_MODEL = "data/island/island.obj";
const char* FIREPLACE_MODEL = "data/Fireplace/fireplace.obj";
const char* HELICOPTER_MODEL = "data/Heli_Bell/Material/Heli.obj";
const char* PALM_MODEL = "data/palm/palm.obj";
const char* FIRE_TEXTURE = "data/fire.png";
const char* WATER_MODEL = "data/water/Water.obj";
const char* ROCK_MODEL = "data/Rock1/stone1.obj";
const char* BENCH_MODEL = "data/bench/bench.obj";

SCommonShaderProgram shaderProgram;

struct bannerShaderProgram {
	GLuint program;
	GLint pos;
	GLint texCoord;
	GLint PVMmatrix;
	GLint time;
	GLint texSampler;
} bannerShaderProgram;

struct skyboxFarPlaneShaderProgram {
	GLuint program;
	GLint screenCoord;
	GLint inversePVmatrix;
	GLint skyboxSampler;
	GLint fogOn;
} skyboxFarPlaneShaderProgram;

struct fireShaderProgram {
	GLuint program;
	GLint pos;
	GLint texCoord;
	GLint PVMmatrix;
	GLint Vmatrix;
	GLint time;
	GLint texSampler;
	GLint frameDuration;
} fireShaderProgram;

bool fogEnabled = true;

/**
* Sends position, direction atd. values to shaders
* \param[in] modelMatrix        Mmatrix
* \param[in] viewMatrix         Vmatrix
* \param[in] projectionMatrix   Pmatrix
*/
void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(shaderProgram.PVMmatrix, 1, GL_FALSE, glm::value_ptr(PVM));

	glUniformMatrix4fv(shaderProgram.Vmatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shaderProgram.Mmatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelMatrix));
	glUniformMatrix4fv(shaderProgram.normalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform1i(shaderProgram.fogOn, fogEnabled);
}

/**
* Sends material values to shaders
* \params[in] ambient, diffuse, specular, shineness   all material uniforms
* \param[in] texture   if object has specific texture, we can use it instead
*/
void setMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, GLuint texture) {

	glUniform3fv(shaderProgram.diffuse, 1, glm::value_ptr(diffuse));
	glUniform3fv(shaderProgram.ambient, 1, glm::value_ptr(ambient));
	glUniform3fv(shaderProgram.specular, 1, glm::value_ptr(specular));
	glUniform1f(shaderProgram.shininess, shininess);

	// if we found texture during loading, we use it
	if (texture != 0) {
		glUniform1i(shaderProgram.useTexture, 1);
		glUniform1i(shaderProgram.texSampler, 0);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	else {
		glUniform1i(shaderProgram.useTexture, 0);
	}
}

// draw func for every object type
//    |
//   \|/
void drawWater(Object* water, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);

	// prepare modelling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), water->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(water->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		waterGeometry->ambient,
		waterGeometry->diffuse,
		waterGeometry->specular,
		waterGeometry->shininess,
		waterGeometry->texture
	);

	// draw geometry

	glBindVertexArray(waterGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, waterGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawIsland(Object* island, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);

	// prepare modelling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), island->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(island->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	setMaterialUniforms(
		islandGeometry->ambient,
		islandGeometry->diffuse,
		islandGeometry->specular,
		islandGeometry->shininess,
		islandGeometry->texture
	);

	// draw geometry

	glBindVertexArray(islandGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, islandGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawFireplace(Object* fireplace, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);

	// prepare modelling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), fireplace->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(fireplace->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	for (size_t i = 0; i < fireplaceGeometry.size(); i++)
	{
		glBindVertexArray(fireplaceGeometry[i]->vertexArrayObject);
		setMaterialUniforms(
			fireplaceGeometry[i]->ambient,
			fireplaceGeometry[i]->diffuse,
			fireplaceGeometry[i]->specular,
			fireplaceGeometry[i]->shininess,
			fireplaceGeometry[i]->texture
		);

		// draw geometry
		glDrawElements(GL_TRIANGLES, fireplaceGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawHeli(HeliObject* helicopter, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);
	glm::mat4 modelMatrix = alignObject(helicopter->position, helicopter->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(helicopter->size, helicopter->size, helicopter->size));
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	for (size_t i = 0; i < heliGeometry.size(); i++)
	{
		glBindVertexArray(heliGeometry[i]->vertexArrayObject);
		setMaterialUniforms(
			heliGeometry[i]->ambient,
			heliGeometry[i]->diffuse,
			heliGeometry[i]->specular,
			heliGeometry[i]->shininess,
			heliGeometry[i]->texture
		);

		// draw geometry
		glDrawElements(GL_TRIANGLES, heliGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawRock(Object* rock, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), rock->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(rock->size, rock->size, rock->size));
	//modelMatrix = glm::rotate(modelMatrix, 180.0f, glm::vec3(1, 0, 0));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	for (size_t i = 0; i < rockGeometry.size(); i++)
	{
		glBindVertexArray(rockGeometry[i]->vertexArrayObject);
		setMaterialUniforms(
			rockGeometry[i]->ambient,
			rockGeometry[i]->diffuse,
			rockGeometry[i]->specular,
			rockGeometry[i]->shininess,
			rockGeometry[i]->texture
		);
		glUniform1i(shaderProgram.useTexture, true);
		// draw geometry
		glDrawElements(GL_TRIANGLES, rockGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawBench(Object* bench, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), bench->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(bench->size));
	modelMatrix = glm::rotate(modelMatrix, 0.1f, glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, 3.14156265f, glm::vec3(0, 0, 1));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	for (size_t i = 0; i < benchGeometry.size(); i++)
	{
		glBindVertexArray(benchGeometry[i]->vertexArrayObject);
		setMaterialUniforms(
			benchGeometry[i]->ambient,
			benchGeometry[i]->diffuse,
			benchGeometry[i]->specular,
			benchGeometry[i]->shininess,
			benchGeometry[i]->texture
		);
		glUniform1i(shaderProgram.useTexture, true);
		// draw geometry
		glDrawElements(GL_TRIANGLES, benchGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawPalm(Object* palm, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);

	// prepare modelling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), palm->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(palm->size));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	for (size_t i = 0; i < palmGeometry.size(); i++)
	{
		glBindVertexArray(palmGeometry[i]->vertexArrayObject);
		setMaterialUniforms(
			palmGeometry[i]->ambient,
			palmGeometry[i]->diffuse,
			palmGeometry[i]->specular,
			palmGeometry[i]->shininess,
			palmGeometry[i]->texture
		);

		// draw geometry
		glDrawElements(GL_TRIANGLES, palmGeometry[i]->numTriangles * 3, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawFire(Fire* fire, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glUseProgram(fireShaderProgram.program);
	glm::mat4 fireRotationMatrix = glm::mat4(viewMatrix[0], viewMatrix[1], viewMatrix[2], glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	fireRotationMatrix = glm::transpose(fireRotationMatrix);
	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), fire->position);
	matrix = glm::scale(matrix, glm::vec3(fire->size));
	matrix = matrix * fireRotationMatrix;
	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(fireShaderProgram.PVMmatrix, 1, GL_FALSE, glm::value_ptr(PVMmatrix));
	glUniformMatrix4fv(fireShaderProgram.Vmatrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform1f(fireShaderProgram.time, fire->currentTime - fire->startTime);
	glUniform1i(fireShaderProgram.texSampler, 0);
	glUniform1f(fireShaderProgram.frameDuration, fire->frameDuration);
	glBindVertexArray(fireMeshGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_2D, fireMeshGeometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, fireMeshGeometry->numTriangles);
	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);
	return;
}

void drawBanner(BannerObject* banner, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(bannerShaderProgram.program);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), banner->position);
	matrix = glm::scale(matrix, glm::vec3(banner->size));

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(bannerShaderProgram.PVMmatrix, 1, GL_FALSE, glm::value_ptr(PVMmatrix));        // model-view-projection
	glUniform1f(bannerShaderProgram.time, banner->currentTime - banner->startTime);
	glUniform1i(bannerShaderProgram.texSampler, 0);

	glBindTexture(GL_TEXTURE_2D, bannerGeometry->texture);
	glBindVertexArray(bannerGeometry->vertexArrayObject);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, bannerGeometry->numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	return;
}

void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(skyboxFarPlaneShaderProgram.program);
	glm::mat4 matrix = projectionMatrix * viewMatrix;
	glm::mat4 viewRotation = viewMatrix;
	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);
	glUniformMatrix4fv(skyboxFarPlaneShaderProgram.inversePVmatrix, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
	glUniform1i(skyboxFarPlaneShaderProgram.skyboxSampler, 0);
	glUniform1i(skyboxFarPlaneShaderProgram.fogOn, fogEnabled);
	glBindVertexArray(skyboxGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxGeometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, skyboxGeometry->numTriangles + 2);
	glBindVertexArray(0);
	glUseProgram(0);
}

void drawCube(Object* cube, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(shaderProgram.program);

	// prepare modelling transform matrix
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), cube->position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(cube->size, cube->size, cube->size));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, -1, -1));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	glUniform3fv(shaderProgram.diffuse, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glUniform3fv(shaderProgram.ambient, 1, glm::value_ptr(glm::vec3(1.0f, 0.1f, 1.0f)));
	glUniform3fv(shaderProgram.specular, 1, glm::value_ptr(glm::vec3(0.7f, 0.1f, 0.7f)));
	glUniform1f(shaderProgram.shininess, 150.8f);
	glUniform1i(shaderProgram.useTexture, false);
	//glUniform1i(shaderProgram.multitexture, false);

	glBindVertexArray(cubeGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, cubeGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);

}
//   /|\
//    |

/**
* Cleans shader programs
*/
void cleanupShaderPrograms(void) {

	pgr::deleteProgramAndShaders(shaderProgram.program);
	pgr::deleteProgramAndShaders(fireShaderProgram.program);
	pgr::deleteProgramAndShaders(bannerShaderProgram.program);
	pgr::deleteProgramAndShaders(skyboxFarPlaneShaderProgram.program);
}

/**
* Creates shaders from shader files
*/
void initializeShaders(void) {
	std::vector<GLuint> shaderList;

	// load and compile shader for lighting (lights & materials)

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "vert.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "frag.frag"));

	// create the program with two shaders
	shaderProgram.program = pgr::createProgram(shaderList);

	// get vertex attributes s, if the shader does not have this uniform -> return -1
	shaderProgram.pos = glGetAttribLocation(shaderProgram.program, "position");
	shaderProgram.normal = glGetAttribLocation(shaderProgram.program, "normal");
	shaderProgram.texCoord = glGetAttribLocation(shaderProgram.program, "texCoord");
	// get uniforms locations
	shaderProgram.PVMmatrix = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	shaderProgram.Vmatrix = glGetUniformLocation(shaderProgram.program, "Vmatrix");
	shaderProgram.Mmatrix = glGetUniformLocation(shaderProgram.program, "Mmatrix");
	shaderProgram.normalMatrix = glGetUniformLocation(shaderProgram.program, "normalMatrix");
	shaderProgram.time = glGetUniformLocation(shaderProgram.program, "time");
	// material
	shaderProgram.ambient = glGetUniformLocation(shaderProgram.program, "material.ambient");
	shaderProgram.diffuse = glGetUniformLocation(shaderProgram.program, "material.diffuse");
	shaderProgram.specular = glGetUniformLocation(shaderProgram.program, "material.specular");
	shaderProgram.shininess = glGetUniformLocation(shaderProgram.program, "material.shininess");
	// texture
	shaderProgram.texSampler = glGetUniformLocation(shaderProgram.program, "texSampler");
	shaderProgram.useTexture = glGetUniformLocation(shaderProgram.program, "material.useTexture");
	// reflector
	shaderProgram.reflectorPosition = glGetUniformLocation(shaderProgram.program, "reflectorPosition");
	shaderProgram.reflectorDirection = glGetUniformLocation(shaderProgram.program, "reflectorDirection");
	shaderProgram.sunOn = glGetUniformLocation(shaderProgram.program, "sunOn");
	shaderProgram.flashOn = glGetUniformLocation(shaderProgram.program, "flashOn");
	shaderProgram.pointlightposition = glGetUniformLocation(shaderProgram.program, "positionPointLight");
	shaderProgram.pointlightison = glGetUniformLocation(shaderProgram.program, "pointLightOn");
	shaderProgram.fogOn = glGetUniformLocation(shaderProgram.program, "fogOn");

	shaderList.clear();

	//fire
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "fire.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fire.frag"));
	//create program 
	fireShaderProgram.program = pgr::createProgram(shaderList);
	// Attribute location
	fireShaderProgram.pos = glGetAttribLocation(fireShaderProgram.program, "position");
	fireShaderProgram.texCoord = glGetAttribLocation(fireShaderProgram.program, "texCoord");
	// Uniform location
	fireShaderProgram.PVMmatrix = glGetUniformLocation(fireShaderProgram.program, "PVMmatrix");
	fireShaderProgram.Vmatrix = glGetUniformLocation(fireShaderProgram.program, "Vmatrix");
	fireShaderProgram.time = glGetUniformLocation(fireShaderProgram.program, "time");
	fireShaderProgram.texSampler = glGetUniformLocation(fireShaderProgram.program, "texSampler");
	fireShaderProgram.frameDuration = glGetUniformLocation(fireShaderProgram.program, "frameDuration");
	//clear the list
	shaderList.clear();


	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "banner.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "banner.frag"));

	// Create the program with two shaders
	bannerShaderProgram.program = pgr::createProgram(shaderList);

	// get position and color attributes locations
	bannerShaderProgram.pos = glGetAttribLocation(bannerShaderProgram.program, "position");
	bannerShaderProgram.texCoord = glGetAttribLocation(bannerShaderProgram.program, "texCoord");
	// get uniforms locations
	bannerShaderProgram.PVMmatrix = glGetUniformLocation(bannerShaderProgram.program, "PVMmatrix");
	bannerShaderProgram.time = glGetUniformLocation(bannerShaderProgram.program, "time");
	bannerShaderProgram.texSampler = glGetUniformLocation(bannerShaderProgram.program, "texSampler");
	shaderList.clear();


	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skybox.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skybox.frag"));

	// create the program with two shaders
	skyboxFarPlaneShaderProgram.program = pgr::createProgram(shaderList);

	// handles to vertex attributes locations
	skyboxFarPlaneShaderProgram.screenCoord = glGetAttribLocation(skyboxFarPlaneShaderProgram.program, "screenCoord");
	// get uniforms locations
	skyboxFarPlaneShaderProgram.skyboxSampler = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "skyboxSampler");
	skyboxFarPlaneShaderProgram.inversePVmatrix = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "inversePVmatrix");
	skyboxFarPlaneShaderProgram.fogOn = glGetUniformLocation(skyboxFarPlaneShaderProgram.program, "fog_isOn");
}

// init geomerty for all objects
//    |
//   \|/
bool loadMultipleMesh(const std::string& fileName, SCommonShaderProgram& shader, std::vector<MeshGeometry*>& geometrys) {
	Assimp::Importer importer;

	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1); // Unitize object in size (scale the model to fit into (-1..1)^3)
																// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);
	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		return false;
	}

	// in this phase we know how many meshes in our loaded scene, we can directly copy its data to opengl ...
	for (size_t i = 0; i < scn->mNumMeshes; i++)
	{
		const aiMesh* mesh = scn->mMeshes[i];

		MeshGeometry* geometry = new MeshGeometry;

		// vertex buffer object, store all vertex positions and normals
		glGenBuffers(1, &((geometry)->vertexBufferObject));
		glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vertexBufferObject);
		glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
																								// first store all vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
		// then store all normals
		glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

		// just texture 0 for now
		float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
		float* currentTextureCoord = textureCoords;

		// copy texture coordinates
		aiVector3D vect;

		if (mesh->HasTextureCoords(0)) {
			// we use 2D textures with 2 coordinates and ignore the third coordinate
			for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
				vect = (mesh->mTextureCoords[0])[idx];
				*currentTextureCoord++ = vect.x;
				*currentTextureCoord++ = vect.y;
			}
		}

		// finally store all texture coordinates
		glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

		// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
		unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
		for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
			indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
			indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
			indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
		}

		// copy our temporary index array to OpenGL and free the array
		glGenBuffers(1, &((geometry)->elementBufferObject));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->elementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

		delete[] indices;

		// copy the material info to MeshGeometry structure
		const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
		aiColor4D color;
		aiString name;
		aiReturn retValue = AI_SUCCESS;

		// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
		mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

		(geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		(geometry)->ambient = glm::vec3(color.r, color.g, color.b);

		if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
			color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
		(geometry)->specular = glm::vec3(color.r, color.g, color.b);

		ai_real shininess, strength;
		unsigned int max;	// changed: to unsigned

		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
			shininess = 1.0f;
		max = 1;
		if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
			strength = 1.0f;
		(geometry)->shininess = shininess * strength;

		(geometry)->texture = 0;

		// load texture image
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			// get texture name 
			aiString path; // filename

			aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			std::string textureName = path.data;

			size_t found = fileName.find_last_of("/\\");
			// insert correct texture file path 
			if (found != std::string::npos) { // not found
											  //subMesh_p->textureName.insert(0, "/");
				textureName.insert(0, fileName.substr(0, found + 1));
			}

			std::cout << "Loading texture file: " << textureName << std::endl;
			(geometry)->texture = pgr::createTexture(textureName);
		}
		CHECK_GL_ERROR();

		glGenVertexArrays(1, &((geometry)->vertexArrayObject));
		glBindVertexArray((geometry)->vertexArrayObject);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
		glBindBuffer(GL_ARRAY_BUFFER, (geometry)->vertexBufferObject);

		glEnableVertexAttribArray(shader.pos);
		glVertexAttribPointer(shader.pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shader.normal);
		glVertexAttribPointer(shader.normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

		glEnableVertexAttribArray(shader.texCoord);
		glVertexAttribPointer(shader.texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
		CHECK_GL_ERROR();

		glBindVertexArray(0);

		(geometry)->numTriangles = mesh->mNumFaces;

		//push newGeometry to vector of geometies
		geometrys.push_back(geometry);
	}
	return true;
}

/** Load mesh using assimp library
 * \param filename [in] file to open/load
 * \param shader [in] vao will connect loaded data to shader
 * \param vbo [out] vertex and normal data |VVVVV...|NNNNN...| (no interleaving)
 * \param eao [out] triangle indices
 * \param vao [out] vao connects data to shader input
 * \param numTriangles [out] how many triangles have been loaded and stored into index array eao
 */
bool loadSingleMesh(const std::string& fileName, SCommonShaderProgram& shader, MeshGeometry** geometry) {
	Assimp::Importer importer;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		*geometry = NULL;
		return false;
	}

	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
	if (scn->mNumMeshes != 1) {
		std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
		*geometry = NULL;
		return false;
	}

	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
	const aiMesh* mesh = scn->mMeshes[0];

	*geometry = new MeshGeometry;

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	// first store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	// then store all normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	// just texture 0 for now
	float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
	float* currentTextureCoord = textureCoords;

	// copy texture coordinates
	aiVector3D vect;

	if (mesh->HasTextureCoords(0)) {
		// we use 2D textures with 2 coordinates and ignore the third coordinate
		for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	// finally store all texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	// copy our temporary index array to OpenGL and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	// copy the material info to MeshGeometry structure
	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;
	aiReturn retValue = AI_SUCCESS;

	// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	(*geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->ambient = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->specular = glm::vec3(color.r, color.g, color.b);

	ai_real shininess, strength;
	unsigned int max;	// changed: to unsigned

	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
		shininess = 1.0f;
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
		strength = 1.0f;
	(*geometry)->shininess = shininess * strength;

	(*geometry)->texture = 0;

	// load texture image
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		// get texture name 
		aiString path; // filename

		aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::string textureName = path.data;

		size_t found = fileName.find_last_of("/\\");
		// insert correct texture file path 
		if (found != std::string::npos) { // not found
		  //subMesh_p->textureName.insert(0, "/");
			textureName.insert(0, fileName.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		(*geometry)->texture = pgr::createTexture(textureName);
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	glEnableVertexAttribArray(shader.pos);
	glVertexAttribPointer(shader.pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shader.normal);
		glVertexAttribPointer(shader.normal, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

	glEnableVertexAttribArray(shader.texCoord);
	glVertexAttribPointer(shader.texCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	(*geometry)->numTriangles = mesh->mNumFaces;

	return true;
}

void initBannerGeometry(MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	(*geometry)->texture = pgr::createTexture(BANNER_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, (*geometry)->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

	//vao
	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	//vbo
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bannerVertexData), bannerVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(bannerShaderProgram.pos);
	glEnableVertexAttribArray(bannerShaderProgram.texCoord);
	// vertices of triangles - start at the beginning of the interlaced array
	glVertexAttribPointer(bannerShaderProgram.pos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	// texture coordinates of each vertices are stored just after its position
	glVertexAttribPointer(bannerShaderProgram.texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = bannerNumQuadVertices;
}

void initSkyboxGeometry(MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	// 2D coordinates of 2 triangles covering the whole screen (NDC), draw using triangle strip
	static const float screenCoords[] = {
	  -1.0f, -1.0f,
	   1.0f, -1.0f,
	  -1.0f,  1.0f,
	   1.0f,  1.0f
	};

	//vao
	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	//vbo
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(skyboxFarPlaneShaderProgram.screenCoord);
	glVertexAttribPointer(skyboxFarPlaneShaderProgram.screenCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();

	(*geometry)->numTriangles = 2;

	//texture
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &((*geometry)->texture));
	glBindTexture(GL_TEXTURE_CUBE_MAP, (*geometry)->texture);

	const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
	GLuint targets[] = {
	  GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		std::string texName = std::string(SKYBOX_PREFIX) + "_" + suffixes[i] + ".jpg";
		std::cout << "Loading cube map texture: " << texName << std::endl;
		if (!pgr::loadTexImage2D(texName, targets[i])) {
			pgr::dieWithError("Skybox cube map loading failed!");
		}
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// unbind the texture (just in case someone will mess up with texture calls later)
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();
}

void fireGeometryInit(MeshGeometry** geometry) {
	*geometry = new MeshGeometry;
	(*geometry)->texture = pgr::createTexture(FIRE_TEXTURE);

	//vao
	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);
	
	//vbo
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertexData), fireVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(fireShaderProgram.pos);
	glVertexAttribPointer(fireShaderProgram.pos, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(fireShaderProgram.texCoord);
	glVertexAttribPointer(fireShaderProgram.texCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);
	(*geometry)->numTriangles = fireNumQuadVertices;
}

void initCubeGeometry(SCommonShaderProgram& shader, MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	// copy our temporary index array to opengl and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * cubeTrianglesCount, cubeIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.pos);
	// vertices of triangles - start at the beginning of the array
	glVertexAttribPointer(shader.pos, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);

	glEnableVertexAttribArray(shader.texCoord);
	glVertexAttribPointer(shader.texCoord, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(shader.normal);
	// color of vertex starts after the position (interlaced arrays)
	glVertexAttribPointer(shader.normal, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = cubeTrianglesCount;
}

/** Initialize vertex buffers and vertex arrays for all objects.
 */
void initializeModels() {
	//initIslandGeometry(shaderProgram, &islandGeometry);

	if (loadSingleMesh(ISLAND_MODEL, shaderProgram, &islandGeometry) != true)
		std::cerr << "Island loading failed." << std::endl;

	if (loadMultipleMesh(FIREPLACE_MODEL, shaderProgram, fireplaceGeometry) != true)
		std::cerr << "Fire loading failed." << std::endl;

	if (loadMultipleMesh(HELICOPTER_MODEL, shaderProgram, heliGeometry) != true)
		std::cerr << "Helicopter loading failed." << std::endl;

	if (loadMultipleMesh(PALM_MODEL, shaderProgram, palmGeometry) != true)
		std::cerr << "Palm loading failed." << std::endl;

	if (loadSingleMesh(WATER_MODEL, shaderProgram, &waterGeometry) != true)
		std::cerr << "Water loading failed." << std::endl;

	if (loadMultipleMesh(ROCK_MODEL, shaderProgram, rockGeometry) != true)
		std::cerr << "Rock loading failed." << std::endl;

	if (loadMultipleMesh(BENCH_MODEL, shaderProgram, benchGeometry) != true)
		std::cerr << "Bench loading failed." << std::endl;

	initCubeGeometry(shaderProgram, &cubeGeometry);

	fireGeometryInit(&fireMeshGeometry);

	initSkyboxGeometry(&skyboxGeometry);

	initBannerGeometry(&bannerGeometry);

	fogEnabled = false;
}
//   /|\
//    |


void cleanupGeometry(MeshGeometry* geometry) {

	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));

	if (geometry->texture != 0)
		glDeleteTextures(1, &(geometry->texture));
}

void cleanupModels() {
	cleanupGeometry(bannerGeometry);
	cleanupGeometry(skyboxGeometry);
}

glm::mat4 alignObject(const glm::vec3& position, const glm::vec3& front, const glm::vec3& up) {

	glm::vec3 z = -glm::normalize(front);
	if (!z.x && !z.y && !z.z)
		z = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 x = glm::normalize(glm::cross(up, z));
	if (!x.x && !x.y && !x.z)
		x = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 y = glm::cross(z, x);
	glm::mat4 matrix = glm::mat4(
		x.x, x.y, x.z, 0.0,
		y.x, y.y, y.z, 0.0,
		z.x, z.y, z.z, 0.0,
		position.x, position.y, position.z, 1.0
	);

	return matrix;
}

void changeFog() {
	fogEnabled = !fogEnabled;
}

void enableFog() {
	fogEnabled = true;
}

void disableFog() {
	fogEnabled = false;
}
