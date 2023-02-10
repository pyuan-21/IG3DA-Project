#version 450 core

void main()
{
	// not really needed, OpenGL does it anyway
	// it will just use gl_Position.z anyway
	// attention! if you want to change depth in fragment shader,
	// don't use GL_DEPTH_COMPONENT, because it will ignore fragmentshader 
	// which mean you have no way to calculate/correct/blur ShadowResult in fragshader!!!
	// Just be careful!!!
}