#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "game_object.h"
#include "linmath.h"

class ball_object:public game_object
{
public:
	GLfloat m_radius;
	GLboolean m_stuck;

	ball_object();
	ball_object(vec2 pos, GLfloat radius, vec2 velocity, texture sprite);

	void move(GLfloat dt, GLuint window_width);
	void reset(vec2 position, vec2 velocity);
};

#endif