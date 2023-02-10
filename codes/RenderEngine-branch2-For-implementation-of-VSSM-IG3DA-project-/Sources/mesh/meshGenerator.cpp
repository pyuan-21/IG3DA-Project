#define _USE_MATH_DEFINES
#include <math.h>
#include "meshGenerator.hpp"
#include <vector>
#include "glm/gtx/quaternion.hpp"
#include "../helpers/utility.hpp"

using namespace std;
using namespace IceRender;

shared_ptr<Mesh> MeshGenerator::GenSphere(const size_t& _hNum, const size_t& _vNum, const float& _radius)
{
	// construct sphere at origin.
	vector<glm::vec3> pos;
	vector<glm::vec3> normals;
	vector<glm::uvec3> indices;

	// using right hand coordinate system. up is y, left is x, toward to user is z
	float dv = M_PI / _vNum; // delta angle in vertical direction
	float dh = 2 * M_PI / _hNum; // delta angle in horizontal direction
	for (size_t i = 0; i <= _vNum; i++)
	{
		float vRadians = dv * i;
		float y = cos(vRadians);
		float xz = sin(vRadians);
		for (size_t j = 0; j <= _hNum; j++)
		{
			float hRadians = dh * j;
			float x = xz * cos(hRadians);
			float z = xz * sin(hRadians);
			glm::vec3 n(x, y, z);
			normals.push_back(n);
			glm::vec3 p = n * _radius;
			pos.push_back(p);

			if (j == _hNum)
				continue;

			// Below, be careful here, don't forgert put parentheses to _i, and _j, otherewise it will become i+1%(_vNum+1) then will get wrong result
#define index(_i, _j) ((_i)%(_vNum+1)*(_hNum+1)+(_j)) 
			// k3 -- k1
			// k4 -- k2
			size_t k1 = index(i, j); // let K1 is the index of pos when this point at (i,j)
			size_t k2 = index(i, j + 1); // similar as above
			size_t k3 = index(i + 1, j);
			size_t k4 = index(i + 1, j + 1);
#undef index
			// two triangles: k1-k4-k3, k1-k2-k4
			indices.push_back(glm::uvec3(k1, k4, k3));
			indices.push_back(glm::uvec3(k1, k2, k4));
		}
	}

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->SetPositions(pos);
	mesh->SetNormals(normals);
	mesh->SetIndices(indices);
	return mesh;
}


shared_ptr<Mesh> MeshGenerator::GenPlane(const float& _width, const float& _height, const glm::vec3& _center, const glm::vec3& _normal)
{
	// just create two triangles to construct a plane
	// first create a plane with normal pointing to positive z-axis
	glm::vec3 tl = _center + glm::vec3(-_width * 0.5, _height * 0.5, 0); // top left
	glm::vec3 tr = _center + glm::vec3(_width * 0.5, _height * 0.5, 0); // top right
	glm::vec3 bl = _center + glm::vec3(-_width * 0.5, -_height * 0.5, 0); // bottom left
	glm::vec3 br = _center + glm::vec3(_width * 0.5, -_height * 0.5, 0); // bottom right
	glm::vec3 n = glm::normalize(_normal);
	if (n != Utility::backV3 && n != Utility::frontV3) {
		float angle = Utility::GetAngleInRadians(Utility::backV3, n);
		glm::quat qua = Utility::GetQuaternion(Utility::backV3, n, angle);
		tl = qua * tl;
		tr = qua * tr;
		bl = qua * bl;
		br = qua * br;
	}

	vector<glm::vec3> pos{ tl,tr,bl,br };
	vector<glm::vec3> normals{ n,n,n,n };

	// indices should be initialized by its normal, to make it counter clockwise with its normal
	vector<glm::uvec3> indices;
	glm::vec3 tl_tr = tr - tl;
	glm::vec3 tl_bl = bl - tl;
	glm::vec3 crs = glm::cross(tl_bl, tl_tr); // rotated axis which rotates 'tl_bl' to 'tl_tr'
	float cosangle = glm::dot(crs, n); // to check which direction is front face(counter clockwise order)
	if (cosangle >= 0)
		indices = { glm::uvec3(0,2,1),glm::uvec3(1,2,3) };
	else
		indices = { glm::uvec3(0,1,2),glm::uvec3(1,3,2) };

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->SetPositions(pos);
	mesh->SetIndices(indices);
	mesh->SetNormals(normals);
	return mesh;
}

shared_ptr<Mesh> MeshGenerator::GenCube(const float& _length)
{
	// _length here is the length of edge
	// each point has tree faces, then it should have three normals
	// to do that, duplicate each point of cube in three times
	float hl = _length / 2; // half length
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::uvec3> indices;

	// positive x
	positions.push_back(glm::vec3(hl, hl, hl));		normals.push_back(Utility::rightV3); // v1
	positions.push_back(glm::vec3(hl, -hl, hl));	normals.push_back(Utility::rightV3); // v2
	positions.push_back(glm::vec3(hl, -hl, -hl));	normals.push_back(Utility::rightV3); // v3
	positions.push_back(glm::vec3(hl, hl, -hl));	normals.push_back(Utility::rightV3); // v4
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 4, positions.size() - 3)); // v4-v1-v2
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 3, positions.size() - 2)); // v4-v2-v3
	// negative x
	positions.push_back(glm::vec3(-hl, hl, -hl));	normals.push_back(Utility::leftV3); // v1
	positions.push_back(glm::vec3(-hl, -hl, -hl));	normals.push_back(Utility::leftV3); // v2
	positions.push_back(glm::vec3(-hl, -hl, hl));	normals.push_back(Utility::leftV3); // v3
	positions.push_back(glm::vec3(-hl, hl, hl));	normals.push_back(Utility::leftV3); // v4
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 4, positions.size() - 3)); // v4-v1-v2
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 3, positions.size() - 2)); // v4-v2-v3
	// positive y
	positions.push_back(glm::vec3(-hl, hl, -hl));	normals.push_back(Utility::upV3); // v1
	positions.push_back(glm::vec3(-hl, hl, hl));	normals.push_back(Utility::upV3); // v2
	positions.push_back(glm::vec3(hl, hl, hl));		normals.push_back(Utility::upV3); // v3
	positions.push_back(glm::vec3(hl, hl, -hl));	normals.push_back(Utility::upV3); // v4
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 4, positions.size() - 3)); // v4-v1-v2
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 3, positions.size() - 2)); // v4-v2-v3
	// negative y
	positions.push_back(glm::vec3(-hl, -hl, hl));	normals.push_back(Utility::downV3); // v1
	positions.push_back(glm::vec3(-hl, -hl, -hl));	normals.push_back(Utility::downV3); // v2
	positions.push_back(glm::vec3(hl, -hl, -hl));	normals.push_back(Utility::downV3); // v3
	positions.push_back(glm::vec3(hl, -hl, hl));	normals.push_back(Utility::downV3); // v4
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 4, positions.size() - 3)); // v4-v1-v2
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 3, positions.size() - 2)); // v4-v2-v3
	// positive z
	positions.push_back(glm::vec3(-hl, hl, hl));	normals.push_back(Utility::backV3); // v1
	positions.push_back(glm::vec3(-hl, -hl, hl));	normals.push_back(Utility::backV3); // v2
	positions.push_back(glm::vec3(hl, -hl, hl));	normals.push_back(Utility::backV3); // v3
	positions.push_back(glm::vec3(hl, hl, hl));		normals.push_back(Utility::backV3); // v4
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 4, positions.size() - 3)); // v4-v1-v2
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 3, positions.size() - 2)); // v4-v2-v3 
	//// negative z
	positions.push_back(glm::vec3(hl, hl, -hl));	normals.push_back(Utility::frontV3); // v1
	positions.push_back(glm::vec3(hl, -hl, -hl));	normals.push_back(Utility::frontV3); // v2
	positions.push_back(glm::vec3(-hl, -hl, -hl));	normals.push_back(Utility::frontV3); // v3
	positions.push_back(glm::vec3(-hl, hl, -hl));	normals.push_back(Utility::frontV3); // v4
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 4, positions.size() - 3)); // v4-v1-v2
	indices.push_back(glm::uvec3(positions.size() - 1, positions.size() - 3, positions.size() - 2)); // v4-v2-v3

	shared_ptr<Mesh> mesh = make_shared<Mesh>();
	mesh->SetPositions(positions);
	mesh->SetNormals(normals);
	mesh->SetIndices(indices);
	return mesh;
}

vector<glm::vec2> MeshGenerator::GenSphereUV(const size_t& _hNum, const size_t& _vNum)
{
	// same method of GenSphere()
	vector<glm::vec2> uv;
	for (size_t i = 0; i <= _vNum; i++)
		for (size_t j = 0; j <= _hNum; j++)
			uv.push_back(glm::vec2(1 - static_cast<float>(j) / _hNum, 1 - static_cast<float>(i) / _vNum));
	return uv;
}

std::vector<glm::vec2> MeshGenerator::GenPlaneUV()
{
	// because Plane has only four points: tl,tr,bl,br
	// therefore, their UV are very obvious.
	return std::vector<glm::vec2>{glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(0, 0), glm::vec2(1, 0)};
}

shared_ptr<Mesh> MeshGenerator::GenMeshFromOFF(const std::string _fileName)
{
	// Load an OFF file. See https://en.wikipedia.org/wiki/OFF_(file_format)
	std::string filePath = "Resources/Models/OFF/" + _fileName;
	std::ifstream s(filePath);
	if (!s.is_open())
	{
		Print("[Error] failed to open: " + filePath);
		return nullptr;
	}

	std::string line;
	int vNum = 0, fNum = 0; // vertices, faces number (Igonre edges here)
	bool usingQuad = false;
	int flag = 0;
	vector<glm::vec3> pos;
	vector<glm::vec3> normals;
	vector<glm::uvec3> indices;
	// not support read color at each vertex
	while (std::getline(s, line))
	{
		if (line.empty())
			continue;
		if (line[0] == '#')
			continue; // skip comment
		if (line.find("OFF") != std::string::npos)
			continue; // OFF letters are optional
		std::istringstream iss(line);
		if (flag == 0)
		{
			if (iss >> vNum >> fNum)
			{
				normals.resize(vNum, Utility::zeroV3);
				flag++;
			}
			else
			{
				Print("[Error] can not read the number of vertice, faces.");
				break;
			}
		}
		else if (flag == 1)
		{
			// reading position
			float x, y, z;
			iss >> x >> y >> z;
			pos.push_back(glm::vec3(x, y, z));
			if (pos.size() == vNum)
				flag++; // enter reading faces
		}
		else if (flag == 2)
		{
			// reading faces
			int pNum; // how many vertex in such face(triangle)
			iss >> pNum;
			if (pNum == 3)
			{
				int v1, v2, v3;
				iss >> v1 >> v2 >> v3;
				indices.push_back(glm::uvec3(v1, v2, v3));
				// normal is computed by using counter clockwise direction
				glm::vec3 e12 = pos[v2] - pos[v1]; // from vertex 1 to vertex 2
				glm::vec3 e13 = pos[v3] - pos[v1]; // from 1 to 3
				glm::vec3 n = glm::normalize(glm::cross(e12, e13));
				normals[v1] += n;
				normals[v2] += n;
				normals[v3] += n;
			}
			else if (pNum == 4)
			{
				usingQuad = true;
				int v1, v2, v3, v4;
				iss >> v1 >> v2 >> v3 >> v4;
				// face v1-v2-v3
				indices.push_back(glm::uvec3(v1, v2, v3));
				// normal is computed by using counter clockwise direction
				glm::vec3 e12 = pos[v2] - pos[v1]; // from vertex 1 to vertex 2
				glm::vec3 e13 = pos[v3] - pos[v1]; // from 1 to 3
				glm::vec3 n = glm::normalize(glm::cross(e12, e13));
				normals[v1] += n;
				normals[v2] += n;
				normals[v3] += n;
				// face v1,v3,v4
				indices.push_back(glm::uvec3(v1, v3, v4));
				// normal is computed by using counter clockwise direction
				glm::vec3 e14 = pos[v4] - pos[v1]; // from 1 to 4
				n = glm::normalize(glm::cross(e13, e14));
				normals[v1] += n;
				normals[v3] += n;
				normals[v4] += n;
			}
			else
			{
				Print("[Error] For now only 3 or 4 vertices in a face can be handled.");
				break;
			}
		}
	}

	// normalized normals
	for (int i = 0; i < normals.size(); i++)
		normals[i] = glm::normalize(normals[i]);

	if (usingQuad)
		fNum *= 2; // when using quad(two triangles) to represent a face, the final face number(here represent the number of triangles) should multiple 2.

	s.close();

	if (pos.size() == vNum && indices.size() == fNum)
	{
		Print("OFF " + _fileName + " has been loaded.");
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->SetPositions(pos);
		mesh->SetNormals(normals);
		mesh->SetIndices(indices);
		return mesh;
	}
	else
	{
		Print("[Error] failed to load OFF: " + _fileName);
		return nullptr;
	}
}


shared_ptr<Mesh> MeshGenerator::GenMeshFromOBJ(const std::string _fileName, vector<glm::vec2>& _uv)
{
	// [Important] TODO: pyuan-21: this function is not finished. For now it just reads vertices, normals.
	// TODO: still need to parse the .mtl, and how to organize the case that different faces group can read different texture but they are sharing the same vertices set.
	// change the rendering process.(For example, a car contains four wheels and a shell. There are two textures for wheels and shell.)
	// Also, it is very slow, and it allocates a lot of memory. To optimize it later.
	
	// Load an OBJ file. See https://en.wikipedia.org/wiki/Wavefront_.obj_file)
	std::string filePath = "Resources/Models/OBJ/" + _fileName;
	std::ifstream s(filePath);
	if (!s.is_open())
	{
		Print("[Error] failed to open: " + filePath);
		return nullptr;
	}

	std::string line;
	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::uvec3> indices;

	// TODO: it seems in .obj, there is no duplicated definition for data. Therefore, we need use temp.
	vector<glm::vec3> pTemp; // pos temp
	vector<glm::vec3> nTemp; // normal temp
	vector<glm::vec2> uvTemp; // uv temp

	// read OBJ file
	while (std::getline(s, line))
	{
		if (line.empty())
			continue;

		// read vertex
		if (line[0] == 'v' && line[1] == ' ')
		{
			std::istringstream iss(line.substr(2, line.size() - 1));
			float x, y, z, w;
			if (iss >> x >> y >> z)
			{
				glm::vec3 v(x, y, z);
				// TODO: use the w to normalize the (x,y,z) ? For now I just do it.
				if (iss >> w)
					v /= w;
				pTemp.push_back(v);
			}
			else
			{
				Print("[Error] can not read the values of vertice.");
				break;
			}
		}

		// read uv coordinate at vertex
		if (line[0] == 'v' && line[1] == 't')
		{
			std::istringstream iss(line.substr(3, line.size() - 1));
			float u, v;
			if (iss >> u)
			{
				glm::vec2 uv(u, 0); // v is 0 by default
				// TODO: for now I ignore the w component.
				if (iss >> v)
					uv.y = v;
				uvTemp.push_back(uv);
			}
			else
			{
				Print("[Error] can not read the values of uv.");
				break;
			}
		}

		// read normal at vertex
		if (line[0] == 'v' && line[1] == 'n')
		{
			std::istringstream iss(line.substr(3, line.size() - 1));
			float x, y, z;
			if (iss >> x >> y >> z)
			{
				glm::vec3 n = glm::normalize(glm::vec3((x, y, z)));
				nTemp.push_back(n);
			}
			else
			{
				Print("[Error] can not read the values of normal.");
				break;
			}
		}

		// read faces, which faces can have more than three points(v/vt/vn, except the first one, all other points are connected to the first one to construct a triangle)
		if (line[0] == 'f' && line[1] == ' ')
		{
			if (line.find("/") != std::string::npos)
			{
				if (line.find("//") != std::string::npos)
				{
					// TODO: not handle case "v1//vn1"
				}
				else
				{
					// TODO: to handle other cases, rewrite the code here
					// for now just handle a squad with "v1/vt1/vn1"
					// refer: https://en.cppreference.com/w/cpp/io/basic_istream/ignore
					size_t index;
					int flag = 0;
					std::vector<glm::uvec3> points; // points on a face, [0]is vertex index, [1] is uv index, [2] is normal index
					std::istringstream iss(line.substr(2, line.size() - 1));
					while (true)
					{
						iss >> index;
						if (iss.fail())
						{
							iss.clear(); // unset failbit
							iss.ignore(1i64, '/'); // skip bad input
						}
						else
						{
							index -= 1; // becasue this index is starting from 1.
							
							if (flag == 0)
								points.push_back(glm::uvec3(index, 0, 0));
							else
								points[points.size() - 1][flag] = index; // flag is 1 or 2

							flag = (flag + 1) % 3;
						}
						if (iss.eof() || iss.bad())
							break;
					}
					// TODO: here only handle create many triangle to construct a face
					// except the first one, all other points are connected to the first one to construct a triangle
					
					for (int i = 0; i < points.size(); i++)
					{
						// starting from i>=3, depulicate the previous one and the first one to construc a triangle
						if (i >= 3)
						{
							// duplicate the first one
							for (int j = 0; j < 3; j++)
							{
								// add position, uv, normal
								size_t index = points[0][j]; // 0 is the first one
								if (j == 0)
									positions.push_back(pTemp[index]);
								else if (j == 1)
									_uv.push_back(uvTemp[index]);
								else
									normals.push_back(nTemp[index]);
							}

							// duplicate the previous one
							for (int j = 0; j < 3; j++)
							{
								// add position, uv, normal
								size_t index = points[i - 1][j]; // (i-1) is previous one
								if (j == 0)
									positions.push_back(pTemp[index]);
								else if (j == 1)
									_uv.push_back(uvTemp[index]);
								else
									normals.push_back(nTemp[index]);
							}
						}

						// add current one
						for (int j = 0; j < 3; j++)
						{
							// add position, uv, normal
							size_t index = points[i][j];
							if (j == 0)
								positions.push_back(pTemp[index]);
							else if (j == 1)
								_uv.push_back(uvTemp[index]);
							else
								normals.push_back(nTemp[index]);
						}

						if (i >= 2)
						{
							// construct triangle
							indices.push_back(glm::uvec3(positions.size() - 3, positions.size() - 2, positions.size() - 1));

							// TODO: there is bug here.(at least for the car model-"hexacar military.obj", the normals are wrong) I don't know whether it is the model's issue or 
							// the parser logic's issue here. Therefore, I just recompute the normals for render it correctly. Later get back to here to figure it out.
							{
								//TODO: pyuan-21, maybe it is no neccessary
								// recompute normals
								glm::vec3 pos1 = positions[positions.size() - 3];
								glm::vec3 pos2 = positions[positions.size() - 2];
								glm::vec3 pos3 = positions[positions.size() - 1];
								glm::vec3 p12 = pos2 - pos1; // from 1 to 2
								glm::vec3 p13 = pos3 - pos1; // from 1 to 3
								glm::vec3 n = glm::normalize(glm::cross(p12, p13));
								for (int m = 1; m <= 3; m++)
									normals[positions.size() - m] = n;
							}
						}
					}
				}
			}
			else
			{
				// TODO: handle only "v1 v2 v3" case
			}
		}
	}

	s.close();

	// verify data
	if (positions.size() > 0 && indices.size() > 0)
	{
		Print("OBJ " + _fileName + " has been loaded.");
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->SetPositions(positions);
		mesh->SetNormals(normals);
		mesh->SetIndices(indices);
		return mesh;
	}
	else
	{
		Print("[Error] failed to load OBJ: " + _fileName);
		return nullptr;
	}
}