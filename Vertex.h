#ifndef __VERTEX_H
#define __VERTEX_H


struct vert
{
	float x, y, z;	//position
	float nx, ny, nz; //vertex normal
	DWORD c;		//color
	float tu, tv;	//texture mapping
	
};


struct PlaneVertex
{
	float x, y, z;
	DWORD c;
	float tu, tv;
};




#endif