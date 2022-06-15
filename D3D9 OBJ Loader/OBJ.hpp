#pragma once

#include <d3dx9.h> // Includes d3d9.h and windows.h

struct OBJInfo
{
	UINT positionsCount;
	UINT uvsCount;
	UINT normalsCount;
	UINT vertexCount;
	UINT faceCount;
	UINT* arr_fVertexCountList;

	~OBJInfo();
};

class OBJ
{
private:
	IDirect3DDevice9* m_pd3dDevice;
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DTexture9* m_pTexture;
	OBJInfo m_info;
public:
	OBJ(IDirect3DDevice9* pd3dDevice, const char* objfile, IDirect3DTexture9* pTexture = nullptr);
	OBJ(IDirect3DDevice9* pd3dDevice, const char* objfile, const char* texturefile = nullptr);
	void Render();
	~OBJ();
};
