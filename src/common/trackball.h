#pragma once
#include <glm/glm.hpp>  
#include <glm/ext.hpp>  
#include <glm/gtx/string_cast.hpp>

class trackball {

	/* a bool variable that indicates if we are currently rotating the trackball*/
	bool is_trackball_dragged;

	/* p0 and p1 points on the sphere */
	glm::vec3 p0, p1;

	/* matrix to transform the scene according to the trackball: rotation only*/
	glm::mat4 rotation_matrix;

	/* matrix to transform the scene according to the trackball: scaling only*/
	glm::mat4  scaling_matrix;
	float scaling_factor;

	/* trackball center */
	glm::vec3 center;

	/* trackball radius */
	float radius;


	/* transform from viewport to window coordinates in thee view reference frame */
	glm::vec2 viewport_to_view(glm::mat4 proj, double pX, double pY) {
		GLint vp[4];
		glm::mat4 proj_inv = glm::inverse(proj);
		glGetIntegerv(GL_VIEWPORT, vp);
		glm::vec4 res(0.f);
		res.x = -1.f + ((float)pX / vp[2])  * (1.f - (-1.f));
		res.y = -1.f + ((vp[3] - (float)pY) / vp[3]) * (1.f - (-1.f));
		res.w = 1.0;
		res = proj_inv*res;
		return glm::vec2(res.x, res.y);
	}

	/* handles the intersection between the position under the mouse and the sphere.
	*/
	bool cursor_sphere_intersection(glm::mat4 proj, glm::mat4 view, glm::vec3 & int_point, double xpos, double ypos) {
		glm::vec2 pos2 = viewport_to_view(proj, xpos, ypos);

		glm::mat4 view_frame = glm::inverse(view);
		glm::vec3 o = view_frame*  glm::vec4(glm::vec3(0.f, 0.f, 0.f), 1.f);
		glm::vec3 d = view_frame*  glm::vec4(glm::vec3(pos2, -2.f), 0.f);

		bool hit = intersection_ray::sphere(int_point, o, d, center, radius);
		if (hit)
			int_point -= center;

		/* this was left to "return true" in class.. It was a gigantic bug with almost never any consequence, except while
		click near the silohuette of the sphere.*/
		return hit;
	}

public:
	void reset() {
		scaling_factor = 1.f;
		scaling_matrix = glm::mat4(1.f);
		rotation_matrix = glm::mat4(1.f);
		//is_trackball_dragged = false;
	}
	void set_center_radius(glm::vec3 c, float r) {
		center = c;
		radius = r;
		reset();
	}


	void mouse_move(glm::mat4 proj, glm::mat4 view, double xpos, double ypos) {
		if (!is_trackball_dragged)
			return;

		if (cursor_sphere_intersection(proj, view, p1, xpos, ypos)) {
			glm::vec3 rotation_vector = glm::cross(glm::normalize(p0), glm::normalize(p1));

			/* avoid near null rotation axis*/
			if (glm::length(rotation_vector) > 0.01) {
				float alpha = glm::asin(glm::length(rotation_vector));
				glm::mat4 delta_rot = glm::rotate(glm::mat4(1.f), alpha, rotation_vector);
				rotation_matrix = delta_rot * rotation_matrix;

				/*p1 becomes the p0 value for the next movement */
				p0 = p1;
			}
		}
	}

	void mouse_press(glm::mat4 proj, glm::mat4 view, double xpos, double ypos) {
		glm::vec3 int_point;
		if (cursor_sphere_intersection(proj, view, int_point, xpos, ypos)) {
			p0 = int_point;
			is_trackball_dragged = true;
		}
	}
	void mouse_release() {
		is_trackball_dragged = false;
	}
	void mouse_scroll(double xoffset, double yoffset)
	{
		scaling_factor *= (float)((yoffset>0) ? 1.1 : 0.97);
		scaling_matrix = glm::scale(glm::mat4(1.f), glm::vec3(scaling_factor, scaling_factor, scaling_factor));
	}

	glm::mat4 matrix() {
		return scaling_matrix*rotation_matrix;
	}
};
