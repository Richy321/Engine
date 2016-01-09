#version 430 core

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_worldPos;

layout(location = 0) out vec4 out_color;

struct DirectionalLight                                                             
{                                                                                   
    vec3 Color;                                                                     
    float AmbientIntensity;                                                         
    float DiffuseIntensity;                                                         
    vec3 Direction;                                                                 
}; 


uniform DirectionalLight gDirectionalLight;
uniform sampler2D gColorMap; 
uniform vec3 gEyeWorldPos;                                                          
uniform float gMatSpecularIntensity;                                                
uniform float gSpecularPower; 

void main()
{
	vec3 normal = normalize(in_normal);
    vec4 ambientColor = vec4(gDirectionalLight.Color, 1.0f) * gDirectionalLight.AmbientIntensity; 
	vec3 lightDirection = -gDirectionalLight.Direction; 
	
	float DiffuseFactor = dot(normal, lightDirection);
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                          
    vec4 SpecularColor = vec4(0, 0, 0, 0);

	if (DiffuseFactor > 0) 
	{                                                        
        DiffuseColor = vec4(gDirectionalLight.Color * gDirectionalLight.DiffuseIntensity * DiffuseFactor, 1.0f);                                               
		
		vec3 VertexToEye = normalize(gEyeWorldPos - in_worldPos);
		vec3 LightReflect = normalize(reflect(gDirectionalLight.Direction, normal));
		float SpecularFactor = dot(VertexToEye, LightReflect);  
		if (SpecularFactor > 0) 
		{
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(gDirectionalLight.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
		}
    }                                                                               
    else 
	{                                                                          
        DiffuseColor = vec4(0, 0, 0, 0);                                            
    }

	out_color = texture2D(gColorMap, in_uv.xy) * (ambientColor + DiffuseColor + SpecularColor);
}