#version 430 core

const int MAX_POINT_LIGHTS = 2;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_worldPos;

layout(location = 0) out vec4 out_color;

struct BaseLight                                                                    
{                                                                                   
    vec3 Color;                                                                     
    float AmbientIntensity;                                                         
    float DiffuseIntensity;                                                         
}; 

struct DirectionalLight                                                             
{                                                                                   
    BaseLight Base;                                                                 
    vec3 Direction;                                                                 
}; 

struct Attenuation                                                                  
{                                                                                   
    float Constant;                                                                 
    float Linear;                                                                   
    float Exp;                                                                      
};                                                                                  
                                                                                    
struct PointLight                                                                           
{                                                                                           
    BaseLight Base;                                                                         
    vec3 Position;                                                                          
    Attenuation Atten;                                                                      
};


uniform DirectionalLight gDirectionalLight;
uniform sampler2D gColorMap; 
uniform vec3 gEyeWorldPos;                                                          
uniform float gMatSpecularIntensity;                                                
uniform float gSpecularPower; 

uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform int gNumPointLights;

vec4 CalcLightInternal(BaseLight light, vec3 lightDirection, vec3 normal)
{
    vec4 ambientColor = vec4(light.Color * light.AmbientIntensity, 1.0f); 
	float DiffuseFactor = dot(normal, -lightDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                          
    vec4 SpecularColor = vec4(0, 0, 0, 0);

	if (DiffuseFactor > 0) 
	{                                                        
        DiffuseColor = vec4(light.Color * light.DiffuseIntensity * DiffuseFactor, 1.0f);                                               
		
		vec3 VertexToEye = normalize(gEyeWorldPos - in_worldPos);
		vec3 LightReflect = normalize(reflect(lightDirection, normal));
		float SpecularFactor = dot(VertexToEye, LightReflect);  
		if (SpecularFactor > 0) 
		{
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
		}
    }                                                                               

	return ambientColor + DiffuseColor + SpecularColor;
}

vec4 CalcDirectionalLight(vec3 Normal)                                                      
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal); 
}  

vec4 CalcPointLight(int Index, vec3 Normal)                                                 
{                                                                                           
    vec3 LightDirection = in_worldPos - gPointLights[Index].Position;                         
    float Distance = length(LightDirection);                                                
    LightDirection = normalize(LightDirection);                                             
                                                                                            
    vec4 Color = CalcLightInternal(gPointLights[Index].Base, LightDirection, Normal);       
    float Attenuation =  gPointLights[Index].Atten.Constant +                               
                         gPointLights[Index].Atten.Linear * Distance +                      
                         gPointLights[Index].Atten.Exp * Distance * Distance;               
                                                                                            
    return Color / Attenuation;                                                             
} 

void main()
{
	vec3 Normal = normalize(in_normal);
	vec4 TotalLight = CalcDirectionalLight(Normal); 

	for (int i = 0 ; i < gNumPointLights; i++) 
	{                                           
        TotalLight += CalcPointLight(i, Normal);                                            
    } 

	out_color = texture2D(gColorMap, in_uv.xy) * TotalLight;
}