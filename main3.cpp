#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include <Eigen>

using namespace Eigen;

// image background color
Vector3f bgcolor(1.0f, 1.0f, 1.0f);

// lights in the scene
std::vector<Vector3f> lightPositions = { Vector3f(0.0, 60, 60)
									   , Vector3f(-60.0, 60, 60)
									   , Vector3f(60.0, 60, 60) };

class Sphere
{
public:
	Vector3f center;  // position of the sphere
	float radius;  // sphere radius
	Vector3f surfaceColor; // surface color

	Sphere(
		const Vector3f& c,
		const float& r,
		const Vector3f& sc) :
		center(c), radius(r), surfaceColor(sc)
	{
	}

	// line vs. sphere intersection (note: this is slightly different from ray vs. sphere intersection!)
	bool intersect(const Vector3f& rayOrigin, const Vector3f& rayDirection, float& t0, float& t1) const
	{
		Vector3f l = center - rayOrigin;
		float tca = l.dot(rayDirection);
		if (tca < 0) return false;
		float d2 = l.dot(l) - tca * tca;
		if (d2 > (radius * radius)) return false;
		float thc = sqrt(radius * radius - d2);
		t0 = tca - thc;
		t1 = tca + thc;

		return true;
	}
};

float max(float x, float y) {
	if (y > x) {
		return y;
	}
	else {
		return x;
	}
}

// diffuse reflection model
Vector3f diffuse(const Vector3f& L, // direction vector from the point on the surface towards a light source
	const Vector3f& N, // normal at this point on the surface
	const Vector3f& diffuseColor,
	const float kd // diffuse reflection constant
)
{
	Vector3f resColor = Vector3f::Zero();

	// TODO: implement diffuse shading model
	// diffuseColor = C	
	// kd = k
		
	resColor = .333 *  diffuseColor * kd * max(N.dot(L), 0);

	return resColor;
}

// Phong reflection model
Vector3f phong(const Vector3f& L,	// direction vector from the point on the surface towards a light source
	const Vector3f& N,				// normal at this point on the surface
	const Vector3f& V,				// direction pointing towards the viewer
	const Vector3f& diffuseColor,
	const Vector3f& specularColor,
	const float kd,					// diffuse reflection constant
	const float ks,					// specular reflection constant
	const float alpha)				// shininess constant
{
	Vector3f resColor = Vector3f::Zero();

	// TODO: implement Phong shading model

	Vector3f R = 2 * N * N.dot(L) - L;
	R.normalize();
	resColor = .333 * specularColor * ks * std::pow(max(R.dot(V), 0), alpha) + diffuse(L,N,diffuseColor,kd);

	return resColor;
}

Vector3f trace(
	const Vector3f& rayOrigin,					// P0
	const Vector3f& rayDirection,				// D
	const std::vector<Sphere>& spheres)			// Scene
{
	Vector3f pixelColor = Vector3f::Zero();		// B

	// TODO: implement ray tracing as described in the homework description

	bool A = false;
	float t0;
	float t1;
	float minimin = INFINITY;
	Vector3f color = bgcolor;
	int index = 0;

	for (int i = 0; i < spheres.size(); i++) {
		bool c = spheres[i].intersect(rayOrigin, rayDirection, t0, t1);

		if (c) {
			if (t0 < minimin)
			{
				minimin = t0;
				color = spheres[i].surfaceColor;
				index = i;
			}

			if (t1 < minimin)
			{
				minimin = t1;
				color = spheres[i].surfaceColor;
				index = i;
			}

			A = true;
		}
	}
	Vector3f sc = Vector3f(1,1,1);
	if (A) {
		for (int i = 0; i < lightPositions.size(); i++) {

			Vector3f newRayOrigin = rayOrigin + (minimin * rayDirection);
			Vector3f L = lightPositions[i] - newRayOrigin;
			L.normalize();
			Vector3f V = rayOrigin - newRayOrigin;
			V.normalize();
			Vector3f N = newRayOrigin - spheres[index].center;
			N.normalize();
			bool checker = false;
			for (int j = 0; j < spheres.size(); j++) {
				bool c = spheres[j].intersect(newRayOrigin, L, t0, t1);
				if (c) {
					checker = true;
				}
			}
			if (!checker) {
				pixelColor += phong(L, N, V, color, sc, 1, 3, 100); //diffuse(L, N, color, 1); // (color * .333);
			}
		}
	}
	else {
		pixelColor = bgcolor;
	}

	return pixelColor;
}

void render(const std::vector<Sphere>& spheres)
{
	unsigned width = 640;
	unsigned height = 480;
	Vector3f* image = new Vector3f[width * height];
	Vector3f* pixel = image;
	float invWidth = 1 / float(width);
	float invHeight = 1 / float(height);
	float fov = 30;
	float aspectratio = width / float(height);
	float angle = tan(M_PI * 0.5f * fov / 180.f);

	// Trace rays
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			float rayX = (2 * ((x + 0.5f) * invWidth) - 1) * angle * aspectratio;
			float rayY = (1 - 2 * ((y + 0.5f) * invHeight)) * angle;
			Vector3f rayDirection(rayX, rayY, -1);
			rayDirection.normalize();
			*(pixel++) = trace(Vector3f::Zero(), rayDirection, spheres);
		}
	}

	// Save result to a PPM image
	std::ofstream ofs("./render.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i)
	{
		const float x = image[i](0);
		const float y = image[i](1);
		const float z = image[i](2);

		ofs << (unsigned char)(std::min(float(1), x) * 255)
			<< (unsigned char)(std::min(float(1), y) * 255)
			<< (unsigned char)(std::min(float(1), z) * 255);
	}

	ofs.close();
	delete[] image;
}

int main(int argc, char** argv)
{
	std::vector<Sphere> spheres;
	// position, radius, surface color
	spheres.push_back(Sphere(Vector3f(0.0, -10004, -20), 10000, Vector3f(0.50, 0.50, 0.50)));
	spheres.push_back(Sphere(Vector3f(0.0, 0, -20), 4, Vector3f(1.00, 0.32, 0.36)));
	spheres.push_back(Sphere(Vector3f(5.0, -1, -15), 2, Vector3f(0.90, 0.76, 0.46)));
	spheres.push_back(Sphere(Vector3f(5.0, 0, -25), 3, Vector3f(0.65, 0.77, 0.97)));
	spheres.push_back(Sphere(Vector3f(-5.5, 0, -13), 3, Vector3f(0.90, 0.90, 0.90)));

	render(spheres);

	return 0;
}
