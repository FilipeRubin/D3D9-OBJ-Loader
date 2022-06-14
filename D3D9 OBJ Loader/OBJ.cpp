#include "OBJ.hpp"
#include <fstream>
#include <string>

#define RELEASE(x) if (x != nullptr) x->Release()

OBJInfo::~OBJInfo()
{
	if (arr_fVertexCountList != nullptr) delete[] arr_fVertexCountList;
}

struct OBJPos
{
	float x, y, z;

	float& operator[] (UINT index)
	{
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
	}
};

struct OBJUV
{
	float u, v;

	float& operator[] (UINT index)
	{
		switch (index)
		{
		case 0:
			return u;
		case 1:
			return v;
		}
	}
};

struct OBJNormal
{
	float nx, ny, nz;

	float& operator[] (UINT index)
	{
		switch (index)
		{
		case 0:
			return nx;
		case 1:
			return ny;
		case 2:
			return nz;
		}
	}
};

struct OBJVertex
{
	float   x,  y,  z;
	float  nx, ny, nz;
	float   u,  v;
};

inline static void LoadInfo(std::fstream& file, OBJInfo& info)
{
	std::string line;
	while (getline(file, line))
	{
		if		(line[0] == 'v' and line[1] == ' ') info.positionsCount++;
		else if (line[0] == 'v' and line[1] == 't') info.uvsCount++;
		else if (line[0] == 'v' and line[1] == 'n') info.normalsCount++;
		else if (line[0] == 'f' and line[1] == ' ')
		{
			for (int i = 1; i < line.size(); i++)
			{
				if (line[i] == ' ')
				{
					info.vertexCount++;
				}
			}
			info.faceCount++;
		}
	}
	info.arr_fVertexCountList = new UINT[info.faceCount];
}

inline static void CreateArrays(OBJInfo& m_info, OBJPos*& positions, OBJUV*& uvs, OBJNormal*& normals)
{
	positions = new OBJPos[m_info.positionsCount];
	uvs = new OBJUV[m_info.uvsCount];
	normals = new OBJNormal[m_info.normalsCount];
}

inline static void LoadPositions(std::fstream& file, OBJPos*& pos)
{
	std::string line;
	int positionIndex = 0;

	while (getline(file, line))
	{
		if (line[0] == 'v' and line[1] == ' ')
		{
			char** posBuffer = new char*[3u];
			for (int i = 0; i < 3; i++) posBuffer[i] = new char[10u];
			for (int i = 0; i < 3; i++) ZeroMemory(posBuffer[i], 10u);

			int elem = 0;
			int elemIndex = 0;

			for (int i = 2; i < line.size(); i++)
			{
				if (line[i] != ' ')
				{
					posBuffer[elem][elemIndex] = line[i];
					elemIndex++;
				}
				else
				{
					elem++;
					elemIndex = 0;
				}
			}

			float convertedElem[3];
			for (int i = 0; i < 3; i++)
			{
				convertedElem[i] = std::stof(posBuffer[i]);
			}

			for (int i = 0; i < 3; i++)
			{
				pos[positionIndex][i] = convertedElem[i];
			}
			positionIndex++;

			for (UINT i = 0u; i < 3u; i++) delete[10u] posBuffer[i];
			delete[3u] posBuffer;
		}
	}
}

inline static void LoadUVs(std::fstream& file, OBJUV*& uvs)
{
	std::string line;
	int uvIndex = 0;

	while (getline(file, line))
	{
		if (line[0] == 'v' and line[1] == 't')
		{
			char** uvBuffer = new char*[2u];

			for (int i = 0; i < 2; i++) uvBuffer[i] = new char[10u];
			for (int i = 0; i < 2; i++) ZeroMemory(uvBuffer[i], 10u);

			int elem = 0;
			int elemIndex = 0;

			for (int i = 3; i < line.size(); i++)
			{
				if (line[i] != ' ')
				{
					uvBuffer[elem][elemIndex] = line[i];
					elemIndex++;
				}
				else
				{
					elem++;
					elemIndex = 0;
				}
			}

			float convertedElem[2];
			for (int i = 0; i < 2; i++)
			{
				convertedElem[i] = std::stof(uvBuffer[i]);
			}

			for (int i = 0; i < 2; i++)
			{
				uvs[uvIndex][i] = i == 0 ? convertedElem[i] : -convertedElem[i];
			}
			uvIndex++;

			for (UINT i = 0u; i < 2u; i++) delete[10u] uvBuffer[i];
			delete[2u] uvBuffer;
		}
	}
}

inline static void LoadNormals(std::fstream& file, OBJNormal*& normals)
{
	std::string line;
	int positionIndex = 0;

	while (getline(file, line))
	{
		if (line[0] == 'v' and line[1] == 'n')
		{
			char** normalsBuffer = new char*[3u];
			for (int i = 0; i < 3; i++) normalsBuffer[i] = new char[10u];
			for (int i = 0; i < 3; i++) ZeroMemory(normalsBuffer[i], 10u);

			int elem = 0;
			int elemIndex = 0;

			for (int i = 3; i < line.size(); i++)
			{
				if (line[i] != ' ')
				{
					normalsBuffer[elem][elemIndex] = line[i];
					elemIndex++;
				}
				else
				{
					elem++;
					elemIndex = 0;
				}
			}

			float convertedElem[3];
			for (int i = 0; i < 3; i++)
			{
				convertedElem[i] = std::stof(normalsBuffer[i]);
			}

			for (int i = 0; i < 3; i++)
			{
				normals[positionIndex][i] = convertedElem[i];
			}
			positionIndex++;

			for (UINT i = 0u; i < 3u; i++) delete[10u] normalsBuffer[i];
			delete[3u] normalsBuffer;
		}
	}
}

inline static void CreateD3DBuffers(IDirect3DDevice9*& pd3dDevice, IDirect3DVertexBuffer9*& pVB, std::fstream& file, OBJInfo& info, OBJPos*& positions, OBJUV*& uvs, OBJNormal*& normals)
{
	OBJVertex* vertices = new OBJVertex[info.vertexCount];
	unsigned long* indexes = new unsigned long[info.vertexCount * 3u];
	ZeroMemory(indexes, (sizeof(indexes) * info.vertexCount * 3u));
	unsigned int indexesIndex = 0u;

	int fCountIndex = 0;

	std::string line;
	while (getline(file, line))
	{
		if (line[0] == 'f' and line[1] == ' ')
		{
			int lineIndex = 2;
			char* elemBuffer = new char[8u];
			ZeroMemory(elemBuffer, 8u);
			int elemIndex = 0;
			int vertexesThisLine = 1;

			while (lineIndex < line.size() + 1u)
			{
				if (line[lineIndex] == ' ')
				{
					vertexesThisLine++;
				}
				if (line[lineIndex] >= 48 and line[lineIndex] <= 57)
				{
					elemBuffer[elemIndex] = line[lineIndex];
					elemIndex++;
				}
				else if ((line[lineIndex] == '/' or line[lineIndex] == ' ' or line[lineIndex] == '\0') and elemIndex > 0)
				{
					indexes[indexesIndex] = std::stoul(elemBuffer);
					indexesIndex++;
					ZeroMemory(elemBuffer, 8u);
					elemIndex = 0;
				}
				lineIndex++;
			}
			info.arr_fVertexCountList[fCountIndex] = vertexesThisLine;
			fCountIndex++;
		}
	}

	for (int i = 0; i < info.vertexCount; i++)
	{
		vertices[i] = { positions[indexes[i * 3] - 1].x, positions[indexes[i * 3] - 1].y, positions[indexes[i * 3] - 1].z, normals[indexes[(i * 3) + 2] - 1].nx, normals[indexes[(i * 3) + 2] - 1].ny, normals[indexes[(i * 3) + 2] - 1].nz , uvs[indexes[(i * 3) + 1] - 1].u, uvs[indexes[(i * 3) + 1] - 1].v };
	}

	pd3dDevice->CreateVertexBuffer(sizeof(OBJVertex) * info.vertexCount, D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, D3DPOOL_DEFAULT, &pVB, nullptr);
	void* pVBOffset;
	pVB->Lock(0, 0, (void**)&pVBOffset, 0);
	memcpy(pVBOffset, vertices, sizeof(OBJVertex) * info.vertexCount);
	pVB->Unlock();

	delete[] indexes;
	delete[] vertices;
}

OBJ::OBJ(IDirect3DDevice9* pd3dDevice, const char* objfile, IDirect3DTexture9* pTexture)
	:m_pd3dDevice(pd3dDevice), m_pVB(nullptr), m_pTexture(pTexture), m_info(OBJInfo())
{
	OBJPos*    positions = nullptr;
	OBJUV*        uvs    = nullptr;
	OBJNormal*  normals  = nullptr;

	std::fstream file;
	file.open(objfile, std::fstream::in);

	LoadInfo(file, m_info);
	CreateArrays(m_info, positions, uvs, normals);
	file.clear();
	file.seekg(0);
	LoadPositions(file, positions);
	file.clear();
	file.seekg(0);
	LoadUVs(file, uvs);
	file.clear();
	file.seekg(0);
	LoadNormals(file, normals);
	file.clear();
	file.seekg(0);
	CreateD3DBuffers(m_pd3dDevice, m_pVB, file, m_info, positions, uvs, normals);

	file.close();

	delete[] positions;
	delete[] uvs;
	delete[] normals;
}

OBJ::OBJ(IDirect3DDevice9* pd3dDevice, const char* objfile, const char* texturefile = nullptr)
	:m_pd3dDevice(pd3dDevice), m_pVB(nullptr), m_pTexture(nullptr), m_info(OBJInfo())
{
	D3DXCreateTextureFromFile(pd3dDevice, texturefile, &m_pTexture);

	OBJPos* positions = nullptr;
	OBJUV* uvs = nullptr;
	OBJNormal* normals = nullptr;

	std::fstream file;
	file.open(objfile, std::fstream::in);

	LoadInfo(file, m_info);
	CreateArrays(m_info, positions, uvs, normals);
	file.clear();
	file.seekg(0);
	LoadPositions(file, positions);
	file.clear();
	file.seekg(0);
	LoadUVs(file, uvs);
	file.clear();
	file.seekg(0);
	LoadNormals(file, normals);
	file.clear();
	file.seekg(0);
	CreateD3DBuffers(m_pd3dDevice, m_pVB, file, m_info, positions, uvs, normals);

	file.close();

	delete[] positions;
	delete[] uvs;
	delete[] normals;
}

void OBJ::Render()
{
	m_pd3dDevice->SetTexture(0, m_pTexture);
	m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(OBJVertex));
	m_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	
	UINT currentIndex = 0u;
	for (UINT i = 0u; i < m_info.faceCount; i++)
	{
		m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, currentIndex, m_info.arr_fVertexCountList[i] - 2u);
		currentIndex += m_info.arr_fVertexCountList[i];
	}
}

OBJ::~OBJ()
{
	RELEASE(m_pVB);
	RELEASE(m_pTexture);
}