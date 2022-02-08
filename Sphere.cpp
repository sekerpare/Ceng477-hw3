#include "Sphere.h"


pair<vector<float>, vector<unsigned int> > generateSphereVerticesAndIndices(float radius, int sectors, int slices, float startx , float starty ,float startz )
{
    vector<float> vertices;
    vector<unsigned int> indices;
    float sectorStep = 2 * glm::pi<float>() / sectors;
    float stackStep = glm::pi<float>() / slices;
    float sectorAngle, stackAngle;
    unsigned int k1, k2;
    
    for (int i = 0; i <= slices; i++)
    {
        stackAngle = glm::pi<float>() / 2 - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);
        for (int j = 0; j <= sectors; j++)
        {
            sectorAngle = j * sectorStep;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            vertices.push_back(x+startx);
            vertices.push_back(y+starty);
            vertices.push_back(z+startz);
            //sonra normalleri pushluyorum
            vertices.push_back(x/radius);
            vertices.push_back(y/radius);
            vertices.push_back(z/radius);
            //sonra texture coordinateleri pushluyorum
            vertices.push_back((float)i/slices);
            vertices.push_back((float)j/sectors);
        }
    }
    
    for (int i = 0; i < slices; i++)
    {
        k1 = i * (sectors + 1);
        k2 = k1 + sectors + 1;
        for (int j = 0; j < sectors; j++)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != slices - 1)
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
            k1++;
            k2++;
        }
    }
    return make_pair(vertices, indices);
}




