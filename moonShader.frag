#version 430

in Data
{
    vec3 Position;
    vec3 Normal;
    vec2 TexCoord;
} data;
in vec3 LightVector;
in vec3 CameraVector;

uniform vec3 lightPosition;
uniform sampler2D TexColor;
uniform sampler2D MoonTexColor;
uniform sampler2D TexGrey;
uniform float textureOffset;

out vec4 FragColor;

vec3 ambientReflectenceCoefficient = vec3(0.5f);
vec3 ambientLightColor = vec3(0.6f);
vec3 specularReflectenceCoefficient= vec3(1.0f);
vec3 specularLightColor = vec3(1.0f);
float SpecularExponent = 10;
vec3 diffuseReflectenceCoefficient= vec3(1.0f);
vec3 diffuseLightColor = vec3(1.0f);


void main()
{
    // Calculate texture coordinate based on data.TexCoord
    vec2 textureCoordinate = vec2(data.TexCoord.x+textureOffset , data.TexCoord.y);
    vec4 texColor = texture(MoonTexColor, textureCoordinate);

    
    diffuseReflectenceCoefficient =   vec3( texColor.x , texColor.y , texColor.z);
    float NdotL  = dot(data.Normal,LightVector );
    float NdotH  = dot(data.Normal,CameraVector);

    vec3 diffuse = diffuseReflectenceCoefficient * diffuseLightColor  * max(0,NdotL);
    vec3 spec    = specularReflectenceCoefficient* specularLightColor * pow(max(0,NdotH),SpecularExponent);
    vec3 ambient = ambientReflectenceCoefficient * ambientLightColor; 
    vec4 color   = vec4(ambient+diffuse+spec,1.0f);

    FragColor    = vec4(color*texColor);
}