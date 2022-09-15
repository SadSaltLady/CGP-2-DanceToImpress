#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

#define F_anim_duration 0.3f

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, bleft, bright, bup, bdown, space;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//hexapod leg to wobble:
	Scene::Transform *hip = nullptr;
	Scene::Transform *upper_leg = nullptr;
	Scene::Transform *lower_leg = nullptr;
	glm::quat hip_base_rotation;
	glm::quat upper_leg_base_rotation;
	glm::quat lower_leg_base_rotation;
	float wobble = 0.0f;
	//-----ADDED -----------
	//add data about floor
	float floor_height = 0.0f;

	//bird references
	Scene::Transform *male_bird = nullptr;
	Scene::Transform *fmale_bird = nullptr;
	Scene::Transform *heart = nullptr;
	Scene::Transform *leg_l = nullptr;
	Scene::Transform *leg_r = nullptr;
	//male bird movement parameters
	float radius = 8.f;
	float m_degree = 0.f;
	float f_degree = 0.f;
	glm::vec3 m_lookat = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat bird_base_rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 leg_l_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 leg_r_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	//smaller animations
	float bobble_body = 0.0f;
	float bobble_force = 0.0f;

	float love_meter = 1.f;
	glm::vec3 heart_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat heart_rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	//timers 
	bool switched = false;
	bool moving = false;
	bool impressed = false;
	float f_anim_timer = 0.0f;
	float heart_timer = 0.0f;
	float impressed_timer = 0.0f;



	//camera:
	Scene::Camera *camera = nullptr;

};
