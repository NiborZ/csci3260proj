#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "Dependencies\glm\glm.hpp"
#include "Dependencies\glm\gtc\matrix_transform.hpp"
//#include "Dependencies\glui\glui.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>

#include "shader_install.h"
#include "Loading_func.h"
#include "Bind_Data.h"

using namespace std;
using glm::vec3;
using glm::mat4;
// ============================= //
// window size
int Win_w, Win_h;
float camera_fov = 45.0;

int  mainWindowID;
GLint programID;
GLint skyboxProgramID;
GLint planetC_programID;
GLint earthProgramID;

GLuint TextureEarth_0;
GLuint TextureEarth_1;
GLuint TexturePlanetC_1;
GLuint TexturePlanetC_0;
GLuint TexturePlanetB;
GLuint TextureObjD;
GLuint TextureObjG;

extern GLuint earthVao;
extern GLuint planetCVao;
extern GLuint planetBVao;
extern GLuint objDVao;
extern GLuint objGVao;

extern int drawEarthSize;
extern int drawPlanetCSize;
extern int drawPlanetBSize;
extern int drawObjDSize;
extern int drawObjGSize;

// view matrix
glm::mat4 common_viewM;
glm::mat4 common_projection;

float earth_innRot_Degree = 0.0f;

// ============================= //
const float M_PI = 3.14159265;
float radius = 30.0f;
float initViewHorizontal = -90.0f;
float initViewVertical = -90.0f;

float cameraX = 0;
float cameraY = 0;
float cameraZ = radius;

float viewRotateDegree[3] = { 0.0f, 0.0f, 0.0f };

float a_brightness = 1.0f;
float d_brightness = 0.0f;
float s_brightness = 0.6f;

void reshape(int w, int h)
{
	Win_w = w;
	Win_h = h;
}

void Mouse_Wheel_Func(int button, int state, int x, int y)
{
	if ((button == 3) || (button == 4))
	{
		if (state == GLUT_UP) return;
		if (button == 3)
		{
			radius -= 1.0f;
			cameraX = radius* cos(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraY = radius* cos(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraZ = radius* sin(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
		}
		else
		{
			radius += 1.0f;
			cameraX = radius* cos(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraY = radius* cos(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraZ = radius* sin(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
		}
	}
}

void LoadAllTextures()
{
	TextureEarth_0 = loadBMP2Texture("texture/earth.bmp");
	TextureEarth_1 = loadBMP2Texture("normal_map/earth_normal.bmp");
	TexturePlanetB = loadBMP2Texture("texture/grass.bmp");
	TexturePlanetC_0 = loadBMP2Texture("texture/glass_a.bmp");
	TexturePlanetC_1 = loadBMP2Texture("texture/apple.bmp");
	TextureObjD = loadBMP2Texture("texture/helicopter.bmp");
	TextureObjG = loadBMP2Texture("texture/brickwall.bmp");
}

void sendDataToOpenGL()
{
	//Load objects and bind to VAO & VBO
	bindEarth("obj/planet.obj");
	bindPlanetB("obj/planet.obj");
	bindPlanetC("obj/planet.obj");
	bindObjD("obj/helicopter2.obj");
	bindObjG("obj/rock.obj");
	// load all textures
	LoadAllTextures();
}

float zLightPos = 0.0f;
float yLightPos = 20.0f;
void set_lighting()
{
	glUseProgram(programID);

	// ambient
	GLint ambientLightUniformLocation = glGetUniformLocation(programID, "ambientLight");
	glm::vec3 ambientLight(a_brightness, a_brightness, a_brightness);
	glUniform3fv(ambientLightUniformLocation, 1, &ambientLight[0]);
	// diffusion
	GLint kd = glGetUniformLocation(programID, "coefficient_d");
	glm::vec3 vec_kd(d_brightness, d_brightness, d_brightness);
	glUniform3fv(kd, 1, &vec_kd[0]);
	// specular
	GLint ks = glGetUniformLocation(programID, "coefficient_s");
	glm::vec3 vec_ks(s_brightness, s_brightness, s_brightness);
	glUniform3fv(ks, 1, &vec_ks[0]);
	// eye position
	GLint eyePositionUniformLocation = glGetUniformLocation(programID, "eyePositionWorld");
	vec3 eyePosition(cameraX, cameraY, cameraZ);
	glUniform3fv(eyePositionUniformLocation, 1, &eyePosition[0]);
	// light position
	GLint lightPositionUniformLocation = glGetUniformLocation(programID, "lightPositionWorld");
	glm::vec3 lightPosition(0.0, yLightPos, zLightPos);
	glUniform3fv(lightPositionUniformLocation, 1, &lightPosition[0]);

	// light color
	GLint lightColorUniformLocation = glGetUniformLocation(programID, "lightColor");
	glm::vec4 lightColor(1.0, 1.0, 1.0, 1.0);
	glUniform4fv(lightColorUniformLocation, 1, &lightColor[0]);
}

void drawEarth(void)
{
	//earth
	GLfloat scale_fact = 3.0f;

	glUseProgram(earthProgramID);

	glBindVertexArray(earthVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(earth_innRot_Degree), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(-8.0f, 6.0f, -10.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(earthProgramID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(earthProgramID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(earthProgramID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID_0 = glGetUniformLocation(earthProgramID, "myTextureSampler_0");
	GLuint TextureID_1 = glGetUniformLocation(earthProgramID, "myTextureSampler_1");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureEarth_0);
	glUniform1i(TextureID_0, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureEarth_1);
	glUniform1i(TextureID_1, 0);

	glDrawArrays(GL_TRIANGLES, 0, drawEarthSize);
}

void drawPlanetC(void)
{
	//Planet C
	GLfloat scale_fact = 1.5f;

	glUseProgram(planetC_programID);

	glBindVertexArray(planetCVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(earth_innRot_Degree), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, -5.0f, 0.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(planetC_programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(planetC_programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(planetC_programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID_0 = glGetUniformLocation(planetC_programID, "myTextureSampler_0");
	GLuint TextureID_1 = glGetUniformLocation(planetC_programID, "myTextureSampler_1");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexturePlanetC_0);
	glUniform1i(TextureID_0, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TexturePlanetC_1);
	glUniform1i(TextureID_1, 1);

	glDrawArrays(GL_TRIANGLES, 0, drawPlanetCSize);
}

void drawPlanetB(void)
{
	//Planet B
	GLfloat scale_fact = 0.2f;

	glUseProgram(programID);

	glBindVertexArray(planetBVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(earth_innRot_Degree), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.5f, +10.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TexturePlanetB);
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE1);

	glDrawArrays(GL_TRIANGLES, 0, drawPlanetBSize);
}

void drawObjD(void)
{
	// object D
	GLfloat scale_fact = 0.1f;

	glUseProgram(programID);

	glBindVertexArray(objDVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 10.0f, +10.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureObjD);
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE1);

	glDrawArrays(GL_TRIANGLES, 0, drawObjDSize);
}

void drawObjG(void)
{
	// object G
	GLfloat scale_fact = 0.5f;

	glUseProgram(programID);

	glBindVertexArray(objGVao);
	glm::mat4 scale_M = glm::scale(glm::mat4(1.0f), glm::vec3(scale_fact));
	glm::mat4 rot_M = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::mat4 trans_M = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -0.5f, 0.0f));
	glm::mat4 Model = trans_M * rot_M * scale_M;

	GLint M_ID = glGetUniformLocation(programID, "MM");
	glUniformMatrix4fv(M_ID, 1, GL_FALSE, &Model[0][0]);
	GLint V_ID = glGetUniformLocation(programID, "VM");
	glUniformMatrix4fv(V_ID, 1, GL_FALSE, &common_viewM[0][0]);
	GLint P_ID = glGetUniformLocation(programID, "PM");
	glUniformMatrix4fv(P_ID, 1, GL_FALSE, &common_projection[0][0]);

	// texture
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureObjG);
	glUniform1i(TextureID, 0);
	glActiveTexture(GL_TEXTURE1);

	glDrawArrays(GL_TRIANGLES, 0, drawObjGSize);
}

void paintGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// ================================ //
	// view matrix
	common_viewM = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	// projection matrix
	common_projection = glm::perspective(camera_fov, 1.0f, 0.1f, 200.0f);
	
	//=== draw ===//
	// set lighting parameters
	set_lighting();
	// draw earth
	drawEarth();
	// draw planet C
	drawPlanetC();
	// draw planet B
	drawPlanetB();
	// draw obj D
	drawObjD();
	drawObjG();

	glutSwapBuffers();
	glutPostRedisplay();
}

void Shaders_Installer()
{
	programID = installShaders("shader/Common.vs", "shader/Common.frag");
	planetC_programID = installShaders("shader/planetC.vs", "shader/planetC.frag");
	skyboxProgramID = installShaders("shader/Skybox.vs", "shader/Skybox.frag");
	earthProgramID = installShaders("shader/Earth.vs", "shader/Earth.frag");

	if (!checkProgramStatus(programID))
		return;
}

void initializedGL(void)
{
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// install all shaders
	Shaders_Installer();
	// load required obj and textures
	sendDataToOpenGL();
}

void timerFunction(int id)
{
	earth_innRot_Degree += 0.3;
	
	glutPostRedisplay();
	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);
}

int main(int argc, char *argv[])
{
	/*Initialization of GLUT library*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	int height = GetSystemMetrics(SM_CYSCREEN)*0.8;
	Win_h = height;
	Win_w = height;
	glutInitWindowSize(Win_w, Win_h);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	mainWindowID = glutCreateWindow("i-Navigation");

	// initialize openGL
	initializedGL();
	// draw
	glutDisplayFunc(paintGL);

	glutMouseFunc(Mouse_Wheel_Func);

	glutTimerFunc(700.0f / 60.0f, timerFunction, 1);

	glutReshapeFunc(reshape);
	/*Enter the GLUT event processing loop which never returns.*/
	glutMainLoop();

	return 0;
}