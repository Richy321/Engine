#pragma once

#include "Ray.h"
#include "../Core/Camera.h"
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")

class RaytracerCPU
{
public:

	RaytracerCPU(float fov, std::shared_ptr<Core::Camera> cam) : camera(cam), fov(fov)
	{
	}

	~RaytracerCPU()
	{
	}

	void Render(std::vector<std::shared_ptr<GameObject>> objects, std::vector<std::shared_ptr<BaseLight>> lights, uint width, uint height)
	{
		vec3 origin(0, 0, 0);

		vec3 *framebuffer = new vec3[width * height];
		vec3 *pix = framebuffer;

		float scale = tan(radians(fov * 0.5f));
		float imageAspectRatio = width / static_cast<float>(height); // assume width > height

		float t = Infinity;


		for (uint32_t j = 0; j < height; ++j)
		{
			for (uint32_t i = 0; i < width; ++i)
			{
				float px = (2 * (i + 0.5) / (float)width - 1) * imageAspectRatio * scale;
				float py = (1 - 2 * (j + 0.5) / (float)height) * scale;

				origin = vec3(vec4(origin, 1.0f) * camera->GetWorldTransform());
				
				vec3 dir = vec3(px, py, -1);
				dir = vec3(vec4(dir, 0.0f) * camera->GetWorldTransform());
				dir = dir - origin;
				dir = normalize(dir);
				//vec3 dir;
				//dir = vec3(vec4(px, py, -1.0f, 0.0f) * camera->GetWorldTransform());
				//dir = normalize(dir);
				*(pix++) = CastRay(origin, dir, objects, lights, width, height, 0);
			}
		}

		std::string filename = "FreeImageResult.bmp";

		SaveToFileFreeImage(filename, framebuffer, width, height);
		delete[] framebuffer;

		OpenImage(filename);
	}

private:
	std::shared_ptr<Core::Camera> camera;
	float fov;

	const float Infinity = std::numeric_limits<float>::max();

	static void SaveToFileFreeImage(std::string filename, vec3 *framebuffer, uint width, uint height)
	{
		FIBITMAP* image = FreeImage_Allocate(width, height, 24);
		if (!image)
			return;

		RGBQUAD colour;		for (uint32_t i = 0; i < width; ++i)
		{
			for (uint32_t j = 0; j < height; j++)
			{
				colour.rgbRed = static_cast<char>(255 * glm::clamp(framebuffer[i].x, 0.0f, 1.0f));
				colour.rgbGreen = static_cast<char>(255 * glm::clamp(framebuffer[i].y, 0.0f, 1.0f));
				colour.rgbBlue = static_cast<char>(255 * glm::clamp(framebuffer[i].z, 0.0f, 1.0f));

				FreeImage_SetPixelColor(image, i, j, &colour);
			}
		}		if (FreeImage_Save(FIF_PNG, image, filename.c_str()))			printf("FreeImage - file saved successfully");

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
			char r = static_cast<char>(255 * glm::clamp(framebuffer[i].x, 0.0f, 1.0f));
			char g = static_cast<char>(255 * glm::clamp(framebuffer[i].y, 0.0f, 1.0f));
			char b = static_cast<char>(255 * glm::clamp(framebuffer[i].z, 0.0f, 1.0f));
			ofs << r << g << b;
		}
		
		ofs.close();
	}


	vec3 CastRay(const vec3& orig, const vec3& dir, 
		const std::vector<std::shared_ptr<GameObject>>& objects, const std::vector<std::shared_ptr<BaseLight>>& lights, 
		const uint& width, const uint& height, uint32 depth)
	{
		vec3 hitColor = (dir + vec3(1)) * 0.5f;
		
		//if (object.intersect(ray, t) && t >= ray.tMin && t <= r.tMax) {


		//vec3 hitColor(1.0f, 1.0f, 0.0f);
		return hitColor;
	}
};

