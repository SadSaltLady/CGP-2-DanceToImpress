#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <random>


//handler to hexapod data
GLuint hexapod_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > hexapod_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("superb.pnct"));
	hexapod_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > hexapod_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("superb.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = hexapod_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = hexapod_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*hexapod_scene) {
	//NOTE: mesh faces +y direction
	//get pointer to body of bird
	for (auto &transform : scene.transforms) {
		if ( transform.name == "m_body") male_bird = &transform;
		else if (transform.name == "f_body") fmale_bird = &transform;
		else if (transform.name == "heart") heart = &transform;
		else if (transform.name == "m_thigh_l") leg_l = &transform;
		else if (transform.name == "m_thigh_r") leg_r = &transform;
	}
	if (male_bird == nullptr) throw std::runtime_error("male BIRD not found.");
	if (fmale_bird == nullptr) throw std::runtime_error("female BIRD not found.");
	if (heart == nullptr) throw std::runtime_error("heart not found.");
	if (leg_l == nullptr) throw std::runtime_error("no left leg");
	if (leg_r == nullptr) throw std::runtime_error("no right leg");

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();
	
	//init some contants:
	floor_height = male_bird->position.z;
	leg_l_pos = leg_l->position;
	leg_r_pos = leg_r->position;

	//move the male bird to be radius away in z direction:
	male_bird->position.y = -radius;
	//move love up!
	heart->position.z += 4.f;
	bird_base_rot = male_bird->rotation;
	heart_scale = heart->scale;

	//very hard coded but no time :(
	camera->transform->position = glm::vec3(-3.51756, 20.4435, 12.2613);
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.downs += 1;
			space.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LEFT) {
			bleft.downs += 1;
			bleft.pressed = true;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			bright.downs += 1;
			bright.pressed = true;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			bup.downs += 1;
			bup.pressed = true;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			bdown.downs += 1;
			bdown.pressed = true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			space.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_LEFT) {
			bleft.pressed = false;
		} else if (evt.key.keysym.sym == SDLK_RIGHT) {
			bright.pressed = false;
		} else if (evt.key.keysym.sym == SDLK_UP) {
			bup.pressed = false;
		} else if (evt.key.keysym.sym == SDLK_DOWN) {
			bdown.pressed = false;
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			camera->transform->rotation = glm::normalize(
				camera->transform->rotation
				* glm::angleAxis(-motion.x * camera->fovy, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::angleAxis(motion.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//with a small chance, rotate the fmale bird
	{
		float chance = 0.005f;

		static glm::quat unshifted = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		static glm::quat shifted = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

		//generate a random number between 0 and 1
		float random = float(rand()) / float(RAND_MAX);
		if (!switched && random < chance) 
		{

			unshifted = bird_base_rot * glm::angleAxis(glm::radians(f_degree), glm::vec3(0.0f, 1.0f, 0.0f));
			f_degree = float(rand()) / float(RAND_MAX) * 360.0f;
			shifted = bird_base_rot * glm::angleAxis(
				glm::radians(f_degree),
				glm::vec3(0.0f, 1.0f, 0.0f));
			switched = true;
			
		}
		else if (switched) 
		{
			f_anim_timer += elapsed;
			if (f_anim_timer > F_anim_duration) {
				f_anim_timer = 0.0f;
				switched = false;
			} else {
				fmale_bird->rotation = glm::mix(unshifted, shifted, f_anim_timer/F_anim_duration);
			}
		}
	}
	//translate bird down with space
	{
		if (bright.pressed) 
		{
			//update degree measure
			m_degree -= 2.f;
			moving = true;
		}
		if (bleft.pressed)
		{
			m_degree += 2.f;	
			moving = true;	
		}
		else
		{
			moving = false;
		}
		if (m_degree > 360) m_degree-= 360.f;
		else if (m_degree < 0) m_degree += 360.f;
	}

	//use bup and bdown to bobble
	{
		float bobble_limit = 20.f;
		bobble_force -= 5.f;
		if (bdown.pressed)
		{
			bobble_force += 10.f;
		}
		if (bobble_force < 0) bobble_force = 0.f;
		if (bobble_force > bobble_limit) bobble_force = bobble_limit;
	}

	//update position of male bird
	{
		float bobble_offset = 40.f;
		float bobble_amount = bobble_offset * (bobble_force) * elapsed * elapsed;
		if (bobble_amount > bobble_offset) bobble_amount = bobble_offset;
		//horizontal
		m_lookat = glm::vec3(std::cos(glm::radians(m_degree)), std::sin(glm::radians(m_degree)), 0.0f);
		//vertical
		male_bird->position = glm::vec3(radius, radius, radius) * m_lookat + 
								glm::vec3(0.f, 0.f, floor_height - bobble_amount);
		//counter act the offset in the legs
		leg_l->position.y = leg_l_pos.y + bobble_amount;
		leg_r->position.y = leg_r_pos.y + bobble_amount;
		//update rotation to face the center
		male_bird->rotation = bird_base_rot * glm::angleAxis(glm::radians(m_degree - 90.f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	//update hopping animation 
	{
		/*
		static leg_transform = 
		//if we are moving, then bobble legs up and down
		if (moving) 
		{

		}
		else 
		{
			//if not moving, move the legs till they are on the ground
		}
		*/
	}

	//update the love meter for the birds <3
	//also win condition
	{
		static float impressed_limit = 3.f; //impress her for 2 seconds
		static float last_height = male_bird->position.z;
		if (impressed_timer < impressed_limit) 
		{
			//love is fleeting every second :( 
			if (love_meter > 0.5f) love_meter -= 0.01f;
			//the closer the difference is to 180, the more the love!
			float diff = std::abs( std::abs(m_degree - f_degree - 90.f) - 180.f);
			float added = 0.02f; 
			//bobble increase love 
			if (last_height != male_bird->position.z) added *= 1.5;
			if (diff < 3.f) love_meter += added;
			
			if (love_meter >= 2.f) 
			{
				love_meter = 2.f;	
				impressed_timer += elapsed;
			}
			//update the shape of the heart
			heart->scale = heart_scale * glm::vec3(love_meter, love_meter, love_meter);
			//rotate the heart
			heart_timer += elapsed;
			last_height = male_bird->position.z;
			heart->rotation = glm::angleAxis(heart_timer, glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else
		{
			//game won
			heart_timer += elapsed;
			//expand heart
			love_meter += elapsed;
			if (love_meter > 5.f) love_meter = 5.f;
			heart->rotation = glm::angleAxis(heart_timer, glm::vec3(0.0f, 0.0f, 1.0f));
			heart->scale = heart_scale * glm::vec3(love_meter, love_meter, love_meter);
		}

	}

	//move camera:
	{
		//combine inputs into a move:
		constexpr float PlayerSpeed = 30.0f;
		glm::vec2 move = glm::vec2(0.0f);
		if (left.pressed && !right.pressed) move.x =-1.0f;
		if (!left.pressed && right.pressed) move.x = 1.0f;
		if (down.pressed && !up.pressed) move.y =-1.0f;
		if (!down.pressed && up.pressed) move.y = 1.0f;

		//make it so that moving diagonally doesn't go faster:
		if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		glm::mat4x3 frame = camera->transform->make_local_to_parent();
		glm::vec3 frame_right = frame[0];
		//glm::vec3 up = frame[1];
		glm::vec3 frame_forward = -frame[2];

		camera->transform->position += move.x * frame_right + move.y * frame_forward;
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	space.downs = 0;
	bup.downs = 0;
	bright.downs = 0;
	bleft.downs = 0;
	bup.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GL_ERRORS(); //print any errors produced by this setup code

	scene.draw(*camera);

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("Mouse motion rotates camera; WASD moves; escape ungrabs mouse",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}
