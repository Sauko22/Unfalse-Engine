
#pragma once
#include "glmath.h"
#include "Color.h"
//#include "Application.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Plane,
	Primitive_Line,
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;
	bool bbox_delete;

protected:
	PrimitiveTypes type;
};


// ============================================
class Plane_Primitive : public Primitive
{
public:
	Plane_Primitive();
	Plane_Primitive(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};

// ============================================
class Line_Primitive : public Primitive
{
public:
	Line_Primitive();
	Line_Primitive(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};