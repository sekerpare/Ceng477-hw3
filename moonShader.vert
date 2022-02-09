#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTex;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 SelfRotationMatrix;
uniform mat4 MVP;

uniform sampler2D TexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;
uniform float orbitDegree;

uniform float heightFactor;
uniform float imageWidth;
uniform float imageHeight;

out Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;


out vec3 LightVector;// Vector from Vertex to Light;
out vec3 CameraVector;// Vector from Vertex to Camera;


void main()
{
    // get orbitDegree value, compute new x, y coordinates
    // there won't be height in moon shader
   
    // set gl_Position variable correctly to give the transformed vertex position
    // compute normal vector

    data.Position = vec3(MVP   * ModelMatrix * vec4(VertexPosition,1.0f) * SelfRotationMatrix);
    data.Normal   = vec3(normalize(vec3(NormalMatrix * ModelMatrix *vec4(VertexNormal  ,0) * SelfRotationMatrix)));
    data.TexCoord = vec2(VertexTex.x,VertexTex.y);

    // set gl_Position variable correctly to give the transformed vertex position
    CameraVector = normalize(vec3(cameraPosition - data.Position ));//bunlari burada hesaplamam gerektigini dusundum. cunku frag kullaniyo
    LightVector  = normalize(vec3(lightPosition  - data.Position ));
    //gl_Position = vec4(0,0,0,0); // this is a placeholder. It does not correctly set the position
    gl_Position = MVP   * ModelMatrix * vec4(VertexPosition,1.0f);

}