#version 430 core

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_uv;

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


void main()
{
	vec3 normal = normalize(in_normal);

    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) * gDirectionalLight.AmbientIntensity; 
	float DiffuseFactor = dot(normal, -gDirectionalLight.Direction);
	vec4 DiffuseColor; 

	if (DiffuseFactor > 0) 
	{                                                        
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *                        
                       gDirectionalLight.DiffuseIntensity *                         
                       DiffuseFactor;                                               
    }                                                                               
    else 
	{                                                                          
        DiffuseColor = vec4(0, 0, 0, 0);                                            
    }

	out_color = texture2D(gColorMap, in_uv.xy) * (AmbientColor + DiffuseColor);
}