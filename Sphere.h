#ifndef SPHERE_H
#define SPHERE_H

#include <vector>
#include <GL/glew.h>
#include <iostream>
#include "glm/glm/ext.hpp"
#include "Shader.h"
#include <vector>
#include "glm/glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <jpeglib.h>
#include <GL/glew.h>
using namespace std;

pair<vector<float>, vector<unsigned int> > generateSphereVerticesAndIndices(float radius, int slices, int sectors,float startx,float starty,float startz);

#endif