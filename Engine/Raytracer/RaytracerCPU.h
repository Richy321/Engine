#pragma once

#include "Ray.h"
#include "../Core/Camera.h"

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
		vec3 orig;

		vec3 *framebuffer = new vec3[width * height];
		vec3 *pix = framebuffer;

		float scale = tan(radians(fov * 0.5f));
		float imageAspectRatio = width / static_cast<float>(height);

		for (uint32_t j = 0; j < height; ++j)
		{
			for (uint32_t i = 0; i < width; ++i)
			{
				vec3 dir;
				*(pix++) = CastRay(orig, dir, objects, lights, width, height, 0);
			}
		}

		SaveToFileFreeImage("FreeImageResult.bmp", framebuffer, width, height);

		delete[] framebuffer;
	}

private:
	std::shared_ptr<Core::Camera> camera;
	float fov;

	static void SaveToFileFreeImage(std::string filename, vec3 *framebuffer, uint width, uint height)
	{
		FIBITMAP* image = FreeImage_Allocate(width, height, 24);
		if (!image)
			return;

		RGBQUAD colour;		for (uint32_t i = 0; i < width; ++i)
		{
			for (uint32_t j = 0; j < height; j++)
			{
				colour.rgbRed = (char)(255 * glm::clamp(framebuffer[i].x, 0.0f, 1.0f));
				colour.rgbGreen = (char)(255 * glm::clamp(framebuffer[i].y, 0.0f, 1.0f));
				colour.rgbBlue = (char)(255 * glm::clamp(framebuffer[i].z, 0.0f, 1.0f));

				FreeImage_SetPixelColor(image, i, j, &colour);
			}
		}		if (FreeImage_Save(FIF_PNG, image, filename.c_str()))			printf("FreeImage - file saved successfully");

		FreeImage_Unload(image);
		//delete image;
	}

	static void SaveToFilePPM(std::string filename, vec3 *framebuffer, uint width, uint height)
	{
		// Save result to a PPM image (keep these flags if you compile under Windows)
		std::ofstream ofs(filename, std::ios::out | std::ios::binary);
		ofs << "P6\n" << width << " " << height << "\n255\n";
		for (uint32_t i = 0; i < height * width; ++i) {
			char r = (char)(255 * glm::clamp(framebuffer[i].x, 0.0f, 1.0f));
			char g = (char)(255 * glm::clamp(framebuffer[i].y, 0.0f, 1.0f));
			char b = (char)(255 * glm::clamp(framebuffer[i].z, 0.0f, 1.0f));
			ofs << r << g << b;
		}
		
		ofs.close();
	}


	vec3 CastRay(const vec3& orig, const vec3& dir, 
		const std::vector<std::shared_ptr<GameObject>>& objects, const std::vector<std::shared_ptr<BaseLight>>& lights, 
		const uint& width, const uint& height, uint32 depth)
	{
		//vec3 hitColor = (dir + vec3(1)) * 0.5f;
		
		vec3 hitColor(1.0f, 0.0f, 0.0f);
		return hitColor;
	}

	inline float clamp(const float &lo, const float &hi, const float &v)
	{
		return std::max(lo, std::min(hi, v));
	}
};

