#include "rasterizer.h"
#include "vector2D.h"
#include "vector3D.h"
#include <cmath>
#include <cstddef>

using namespace std;

namespace CGL {

	RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm, size_t width, size_t height,
		unsigned int sample_rate) {
		this->psm = psm;
		this->lsm = lsm;
		this->width = width;
		this->height = height;
		this->sample_rate = sample_rate;

		sample_buffer.resize(width * height * sample_rate, Color::White);
	}

	// Used by rasterize_point and rasterize_line
	void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
		for (int s = 0; s < sample_rate; s++) {
			sample_buffer[sample_rate * (y * width + x) + s] = c;
		}
	}

	// Used by rasterize_triangle
	void RasterizerImp::fill_pixel(size_t x, size_t y, size_t s, Color c) {
		sample_buffer[sample_rate * (y * width + x) + s] = c;
	}

	// Rasterize a point: simple example to help you start familiarizing
	// yourself with the starter code.
	void RasterizerImp::rasterize_point(float x, float y, Color color) {
		// fill in the nearest pixel
		int sx = (int)floor(x);
		int sy = (int)floor(y);

		// check bounds
		if (sx < 0 || sx >= width) return;
		if (sy < 0 || sy >= height) return;

		fill_pixel(sx, sy, color);
		return;
	}

	// Rasterize a line.
	void RasterizerImp::rasterize_line(float x0, float y0, float x1, float y1, Color color) {
		if (x0 > x1) {
			swap(x0, x1); swap(y0, y1);
		}

		float pt[] = { x0,y0 };
		float m = (y1 - y0) / (x1 - x0);
		float dpt[] = { 1,m };
		int steep = abs(m) > 1;
		if (steep) {
			dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
			dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
		}

		while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
			rasterize_point(pt[0], pt[1], color);
			pt[0] += dpt[0]; pt[1] += dpt[1];
		}
	}

	// Rasterize a triangle.
	void RasterizerImp::rasterize_triangle(float x0, float y0, float x1, float y1, float x2, float y2, Color color) {
		// TODO: Task 1: Implement basic triangle rasterization here, no supersampling

		// Bounding box limits for triangle
		float x_list[] = { x0,x1,x2 };
		float y_list[] = { y0,y1,y2 };
		// x pixel bounds
		int minx = (int)floor(*std::min_element(x_list, x_list+3));
		int maxx = (int)ceil(*std::max_element(x_list, x_list+3));
		// y pixel bounds
		int miny = (int)floor(*std::min_element(y_list, y_list+3));
		int maxy = (int)ceil(*std::max_element(y_list, y_list+3));

		// Iterate through all x and y values within our bound
		for (int y = miny; y < maxy; y++) {
			for (int x = minx; x < maxx; x++) {

				// TODO: Task 2: Update to implement super-sampled rasterization
				float sr = sqrt(sample_rate);
				for (float j = 0; j < sr; j++) {
					float y_pos = (float)y + (j + 0.5) / sr;

					for (int i = 0; i < sr; i++) {
						float x_pos = (float)x + (i + 0.5) / sr;
						// Edge functions that tells us what side of each edge our point is on
						float E01 = (x_pos - x0)*(y1 - y0) - (y_pos - y0)*(x1 - x0);
						float E12 = (x_pos - x1)*(y2 - y1) - (y_pos - y1)*(x2 - x1);
						float E20 = (x_pos - x2)*(y0 - y2) - (y_pos - y2)*(x0 - x2);

						// Our point lies on the same side of all sides (2 conditions depending on orientation)
						bool same_side = (E01 > 0 && E12 > 0 && E20 > 0) || (E01 < 0 && E12 < 0 && E20 < 0);

						if (same_side) {
							fill_pixel(x, y, i + sr * j, color);
						}
					}
				}
			}
		}
	}


	void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
		float x1, float y1, Color c1,
		float x2, float y2, Color c2)
	{
		// TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
		// Hint: You can reuse code from rasterize_triangle



	}


	void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
		float x1, float y1, float u1, float v1,
		float x2, float y2, float u2, float v2,
		Texture& tex)
	{
		// TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
		// TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
		// Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle




	}

	void RasterizerImp::set_sample_rate(unsigned int rate) {
		// TODO: Task 2: You may want to update this function for supersampling support
		this->sample_rate = rate;
		this->sample_buffer.resize(width * height * sample_rate, Color::White);
	}


	void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer, size_t width, size_t height)
	{
		// TODO: Task 2: You may want to update this function for supersampling support
		this->width = width;
		this->height = height;
		this->rgb_framebuffer_target = rgb_framebuffer;
		this->sample_buffer.resize(width * height * sample_rate, Color::White);
	}


	void RasterizerImp::clear_buffers() {
		std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
		std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
	}


	// This function is called at the end of rasterizing all elements of the
	// SVG file.  If you use a supersample buffer to rasterize SVG elements
	// for antialising, you could use this call to fill the target framebuffer
	// pixels from the supersample buffer data.
	//
	void RasterizerImp::resolve_to_framebuffer() {
		// TODO: Task 2: You will likely want to update this function for supersampling support
		for (int x = 0; x < width; ++x) {
			for (int y = 0; y < height; ++y) {
				// Sum up rgb values for the 
				Color avg_color = Color::Black;
				for (int s = 0; s < sample_rate; s++) {
					avg_color += sample_buffer[sample_rate * (y * width + x) + s];
				}
				
				// Fill target framebuffer with average color 
				this->rgb_framebuffer_target[3 * (y * width + x)] = avg_color.r / sample_rate * 255;
				this->rgb_framebuffer_target[3 * (y * width + x) + 1] = avg_color.g / sample_rate * 255;
				this->rgb_framebuffer_target[3 * (y * width + x) + 2] = avg_color.b / sample_rate * 255;
			}
		}
	}

	Rasterizer::~Rasterizer() { }


}// CGL
