#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTex;

uniform vec3 lightPosition; //
uniform vec3 cameraPosition;//

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ModelMatrix;
uniform mat4 MVP; //

uniform sampler2D TexColor;//
uniform sampler2D TexGrey; //
uniform float textureOffset; // BU NE ????????

uniform float heightFactor;//
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
    // Calculate texture coordinate based on data.TexCoord
    vec2 textureCoordinate = vec2(VertexTex.x + textureOffset ,VertexTex.y);
    vec4 texColor = texture(TexGrey, textureCoordinate);

    // get texture value, compute height
    vec3 height   = vec3(heightFactor * texColor.r * VertexNormal);//sadece red channeli ekleyin demis odevde
    vec4 Position = vec4(VertexPosition+height , 1.0f);//homogeneous coordinate

    // compute normal vector
    data.Position = vec3(MVP   * ModelMatrix * Position);
    data.Normal   = vec3(normalize(vec3(NormalMatrix * ModelMatrix * vec4(VertexNormal  ,1.0f))));
    data.TexCoord = vec2(VertexTex.x,VertexTex.y);

    // set gl_Position variable correctly to give the transformed vertex position
    CameraVector = normalize(vec3(cameraPosition - data.Position ));//bunlari burada hesaplamam gerektigini dusundum. cunku frag kullaniyo
    LightVector  = normalize(vec3(lightPosition  - data.Position ));
    //gl_Position = vec4(0,0,0,0); // this is a placeholder. It does not correctly set the position
    gl_Position = MVP * ModelMatrix *Position;
}