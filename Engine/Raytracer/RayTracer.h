#pragma once

#include "Ray.h"
#include "../Core/Camera.h"
#include <shellapi.h>
#include "../Material.h"
#include "../MaterialComponent.h"
#pragma comment(lib, "shell32.lib")

class RayTracer
{
public:

	RayTracer(float fov, std::shared_ptr<Core::Camera> cam) : camera(cam), fov(fov)
	{
	}

	~RayTracer()
	{
	}

	void Render(std::vector<std::shared_ptr<GameObject>> objects, std::vector<std::shared_ptr<BaseLight>> lights, uint width, uint height)
	{
		vec3 origin(0, 0, 0);

		vec3 *framebuffer = new vec3[width * height];
		vec3 *pix = framebuffer;

		float scale = tan(radians(fov * 0.5f));
		float imageAspectRatio = width / static_cast<float>(height); // assume width > height
		float invWidth = 1 / float(width), invHeight = 1 / float(height);

		for (uint32_t j = 0; j < height; ++j)
		{
			for (uint32_t i = 0; i < width; ++i)
			{
				float px = (2 * (i + 0.5f) * invWidth - 1) * imageAspectRatio * scale;
				float py = (1 - 2 * ((j + 0.5f) * invHeight))* scale;

				origin = vec3(vec4(origin, 0.0f) * camera->GetWorldTransform());
				
				vec3 dir = vec3(px, py, -1);
				dir = vec3(vec4(dir, 0.0f) * camera->GetWorldTransform());
				dir = dir - origin;
				dir = normalize(dir);

				Ray ray(origin, dir);
				*(pix++) = CastRay(ray, objects, lights, 0);
			}
		}

		std::string filename = "FreeImageResult.bmp";
		SaveToFileFreeImage(filename, framebuffer, width, height);
		
		std::string filenamePPM = "FreeImageResult2.ppm";
		SaveToFilePPM(filenamePPM, framebuffer, width, height);

		delete[] framebuffer;

		OpenImage(filename);
	}

private:
	std::shared_ptr<Core::Camera> camera;
	float fov;
	uint MaxRecursionDepth = 5;

	const float Infinity = std::numeric_limits<float>::max();
	vec3 defaultColour = vec3(2);

	static void SaveToFileFreeImage(std::string filename, vec3 *framebuffer, uint width, uint height)
	{
		FIBITMAP* image = FreeImage_Allocate(width, height, 24);
		if (!image)
			return;

		for (uint32_t j = 0; j < height; j++)
		{
			for (uint32_t i = 0; i < width; ++i)
			{
				RGBQUAD colour;
				colour.rgbRed = 255 * framebuffer[j* width + i].x;
				colour.rgbGreen = 255 * framebuffer[j* width + i].y;
				colour.rgbBlue = 255 * framebuffer[j* width + i].z;

				FreeImage_SetPixelColor(image, i, height - j -1, &colour);
			}
		}

		if (FreeImage_Save(FIF_BMP, image, filename.c_str()))
			printf("FreeImage - file saved successfully");

		FreeImage_Unload(image);
	}

	static void OpenImage(std::string filename)
	{
		ShellExecuteA(nullptr, nullptr, filename.c_str(), nullptr, nullptr, SW_SHOW);
	}

	static void SaveToFilePPM(std::string filename, vec3 *framebuffer, uint width, uint height)
	{
		// Save result to a PPM image
		std::ofstream ofs(filename, std::ios::out | std::ios::binary);
		ofs << "P6\n" << width << " " << height << "\n255\n";
		for (uint32_t i = 0; i < height * width; ++i) {
			ofs << (unsigned char)(std::min(float(1), framebuffer[i].x) * 255) <<
				(unsigned char)(std::min(float(1), framebuffer[i].y) * 255) <<
				(unsigned char)(std::min(float(1), framebuffer[i].z) * 255);
		}
		ofs.close();
	}


	bool Trace(const Ray& ray, const std::vector<std::shared_ptr<GameObject>>& objects, float& tnear, std::shared_ptr<SphereColliderComponent>& closestSphere)
	{
		for (auto o : objects)
		{
			std::shared_ptr<IComponent> component = o->GetComponentByType(IComponent::Collider);
			if (component != nullptr)
			{
				std::shared_ptr<ICollider> collider = std::dynamic_pointer_cast<ICollider>(component);
				if (collider != nullptr && collider->GetColliderType() == ICollider::ColliderType::SphereCollider)
				{
					std::shared_ptr<SphereColliderComponent> sphereColliderComponent = std::dynamic_pointer_cast<SphereColliderComponent>(collider);
					if (sphereColliderComponent != nullptr)
					{
						float t0 = Infinity;
						float t1 = Infinity;
						if (RaySphereIntersect(sphereColliderComponent, ray, t0, t1))
						{
							if (t0 < 0)
								t0 = t1;
							if (t0 < tnear)
							{
								tnear = t0;
								closestSphere = sphereColliderComponent;
							}
						}
					}
				}
			}
		}

		return closestSphere != nullptr;
	}

	vec3 CastRay(const Ray& primaryRay, const std::vector<std::shared_ptr<GameObject>>& objects, const std::vector<std::shared_ptr<BaseLight>>& lights, uint32 depth)
	{
		vec3 surfaceColour(0.0f);

		float tnear = Infinity;
		std::shared_ptr<SphereColliderComponent> closestSphere = nullptr;

		//early out - over our recursion depth limit, return default background colour
		if(depth > MaxRecursionDepth)
			return defaultColour;

		if (!Trace(primaryRay, objects, tnear, closestSphere))
		{
			//early out - return default background colour if no intersection
			return defaultColour;
		}

		std::shared_ptr<MaterialComponent> sphereMaterial = nullptr;
		std::shared_ptr<IComponent> tmpComp = closestSphere->GetParentGameObject().lock()->GetComponentByType(IComponent::RenderMaterial);
		if(tmpComp != nullptr)
		{
			sphereMaterial = std::dynamic_pointer_cast<MaterialComponent>(tmpComp);
		}

		vec3 intersectionPoint = primaryRay.origin + primaryRay.direction * tnear;
		vec3 intersectionNormal = intersectionPoint - closestSphere->Position();
		intersectionNormal = normalize(intersectionNormal);

		float bias = 1e-4; // add small bias to the point from which we will raytrace
		bool inside = false;
		if (dot(primaryRay.direction, intersectionNormal) > 0)
		{
			intersectionNormal = -intersectionNormal;
			inside = true;
		}
		//check is inside object
		if (dot(primaryRay.direction, intersectionNormal) > 0)
		{
			intersectionNormal = -intersectionNormal;
			inside = true;
		}

		if (sphereMaterial != nullptr)
		{
			if(sphereMaterial->GetMaterial()->transparency > 0 || sphereMaterial->GetMaterial()->reflection > 0)
			{
				float facingratio = -dot(primaryRay.direction, intersectionNormal);
				float fresneleffect = Mix(pow(1.0f - facingratio, 3.0f), 1.0f, 0.1f);
				
				// compute reflection direction
				vec3 refldir = Reflect(primaryRay.direction, intersectionNormal);
				refldir = normalize(refldir);
				float reflNear = 0;
				vec3 reflection = CastRay(Ray(intersectionPoint + intersectionNormal * bias, refldir), objects, lights, depth + 1);
				vec3 refraction(0.0f);
				// if the sphere is also transparent compute refraction ray (transmission)
				if (sphereMaterial->GetMaterial()->transparency) 
				{
					float ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
					float cosi = -dot(intersectionNormal, primaryRay.direction);
					float k = 1 - eta * eta * (1 - cosi * cosi);
					vec3 refrdir = primaryRay.direction * eta + intersectionNormal * (eta *  cosi - sqrt(k));
					refrdir = normalize(refrdir);
					refraction = CastRay(Ray(intersectionPoint - intersectionNormal * bias, refrdir), objects, lights, depth + 1);
				}
				// the result is a mix of reflection and refraction (if the sphere is transparent)
				surfaceColour = (
					reflection * fresneleffect +
					refraction * (1 - fresneleffect) * sphereMaterial->GetMaterial()->transparency) * sphereMaterial->GetMaterial()->colour;
			}
			else
			{
				//fully opaque object/diffuse
				vec3 lightDirection;
				vec3 lightAmount(0.0f);
				vec3 specularColour(0.0f);

				vec3 shadowPointOrigin = (dot(primaryRay.direction, intersectionNormal) < 0) ?
					intersectionPoint + intersectionNormal * bias :
					intersectionPoint - intersectionNormal * bias;

				for each (std::shared_ptr<BaseLight> light in lights)
				{
					switch (light->GetLightType())
					{
					case BaseLight::Base:
						//not a valid light
						break;
					case BaseLight::Point:
					{
						std::shared_ptr<PointLight> pointLight = std::dynamic_pointer_cast<PointLight>(light);
						if (pointLight != nullptr)
						{
							vec3 transmission(1.0f);

							lightDirection = pointLight->Position - intersectionPoint;
							float lightDistance2 = dot(lightDirection, lightDirection);
							lightDirection = normalize(lightDirection);
							float lightDotNorm = std::max(0.0f, dot(lightDirection, intersectionNormal));

							Ray shadowRay(shadowPointOrigin, lightDirection, Ray::Shadow);

							bool isInShadow = false;
							float tNearShadow = Infinity;
							std::shared_ptr<SphereColliderComponent> closestSphereShadow;
							isInShadow = Trace(shadowRay, objects, tNearShadow, closestSphereShadow) && tNearShadow * tNearShadow < lightDistance2;

							if (isInShadow)
								transmission = vec3(0.0f);
	
							//Phong lighting 
							lightAmount += (1 - isInShadow) * pointLight->DiffuseIntensity * lightDotNorm;
							vec3 reflectionDir = Reflect(-lightDirection, intersectionNormal);
							specularColour += powf(std::max(0.0f, -dot(reflectionDir, primaryRay.direction)), sphereMaterial->GetMaterial()->specularExponent) * pointLight->DiffuseIntensity;

							//diffuse lighting
							//surfaceColour += sphereMaterial->GetMaterial()->colour * transmission *
							//	std::max(float(0), dot(intersectionNormal, lightDirection));
						}
					}
						break;
					default:
						break;
					}
				}
				surfaceColour = lightAmount * sphereMaterial->GetMaterial()->colour * sphereMaterial->GetMaterial()->Kd + specularColour * sphereMaterial->GetMaterial()->Ks;
			}
		}
		else
		{
			surfaceColour = vec3(Colours_RGBA::HotPink);
		}
		return surfaceColour;
	}

	vec3 Reflect(const vec3& i, const vec3& n)
	{
		return i - 2 * dot(i, n) * n;
	}

	float Mix(const float &a, const float &b, const float &mix)
	{
		return b * mix + a * (1 - mix);
	}

#pragma region Intersect calculations

	bool RaySphereIntersect(const std::shared_ptr<SphereColliderComponent>& sphere, const Ray& ray, float &t0, float &t1) const
	{
		vec3 l = sphere->Position() - ray.origin;
		float tca = dot(l, ray.direction);

		if (tca < 0)
			return false;
		float d2 = (dot(l, l)) - (tca * tca);

		if (d2 > sphere->GetRadius2())
			return false;

		float thc = sqrt(sphere->GetRadius2() - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}

	bool RayBoxIntersect(const std::shared_ptr<BoxColliderComponent>& box) const
	{
		return false;
	}

	bool RayPlaneIntersect() const
	{
		return false;
	}

#pragma endregion

};

