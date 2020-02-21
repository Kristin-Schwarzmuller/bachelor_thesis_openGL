// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <OBJModel.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

//#include <DataTypes.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
//	===========================================================================================================================================================================================		#####



// Usings																																														   Usings
//	===========================================================================================================================================================================================		#####
using std::vector;
using std::ifstream;
using std::string;

//using namespace mgraphics::datatypes;
//	===========================================================================================================================================================================================		#####



// Implementierung																																										  Implementierung
//	===========================================================================================================================================================================================		#####
namespace mgraphics
{
	namespace objmodel
	{
		// Structs																																												  Structs
		//	===================================================================================================================================================================================		#####
		template<typename T>
		struct vec2
		{
			T x;
			T y;

			bool reached;

			vec2() : reached(false)
			{

			}
		};

		template<typename T>
		struct vec3
		{
			T x;
			T y;
			T z;

			bool reached;

			vec3() : reached(false)
			{

			}
		};

		struct triangle_face
		{
			vec3<int> v1, v2, v3;
		};

		struct nData
		{
			GLint vertices, normals, uvs, faces, tangents, bitangents;
		};
		//	===================================================================================================================================================================================		#####




		// OBJModel																																												 OBJModel
		//	===================================================================================================================================================================================		#####
		OBJModel::OBJModel(const string& modelpath, bool withTangentsAndBitangents)
		{
			heavyLoading = withTangentsAndBitangents;

			int sizeOfModelString = modelpath.size();
			
			if(modelpath.find(".binobj", sizeOfModelString - 7, 7) != modelpath.npos || modelpath.find(".BINOBJ", sizeOfModelString - 7, 7) != modelpath.npos)
			{
				ifstream input(modelpath, std::ios::in | std::ios::binary);

				if(!input)
				{
					std::cout << "Model not found!" << std::endl;
					exit(0xaffe);
				}


				nData ndata;

				input.read((char *)&ndata, sizeof(nData));

				faces = ndata.faces;


				normals.reserve(ndata.normals);
				uvs.reserve(ndata.uvs);
				vertices.reserve(ndata.vertices);

				if(heavyLoading)
				{
					tangents.reserve(ndata.tangents);
					bitangents.reserve(ndata.bitangents);
				}

				long offset = sizeof(nData);

				std::unique_ptr<float[]> verts = std::make_unique<float[]>(ndata.vertices);
				std::unique_ptr<float[]> uvptr = std::make_unique<float[]>(ndata.uvs);
				std::unique_ptr<float[]> normals = std::make_unique<float[]>(ndata.normals);

				input.seekg(offset, std::ios::beg);
				input.read((char *) verts.get(), ndata.vertices * sizeof(float));

				offset += ndata.vertices * sizeof(float);

				input.seekg(offset, std::ios::beg);
				input.read((char *) uvptr.get(), ndata.uvs * sizeof(float));

				offset += ndata.uvs * sizeof(float);

				input.seekg(offset, std::ios::beg);
				input.read((char *) normals.get(), ndata.normals * sizeof(float));

				if(heavyLoading)
				{
					std::unique_ptr<float[]> tangs(new float[ndata.tangents * sizeof(float)]);
					std::unique_ptr<float[]> bitangs(new float[ndata.bitangents * sizeof(float)]);

					offset += ndata.normals * sizeof(float);

					input.seekg(offset, std::ios::beg);
					input.read((char *) tangs.get(), ndata.tangents * sizeof(float));

					offset += ndata.tangents * sizeof(float);

					input.seekg(offset, std::ios::beg);
					input.read((char *) bitangs.get(), ndata.bitangents * sizeof(float));
				}
			}
			else if(modelpath.find(".obj", sizeOfModelString - 4, 4) != modelpath.npos || modelpath.find(".OBJ", sizeOfModelString - 4, 4) != modelpath.npos)
			{
				vector<vec3<float>> vertex_data;
				vector<vec3<float>> normal_data;
				vector<vec2<float>> uv_data;

				vector<vec3<float>> tangent_data;
				vector<vec3<float>> bitangent_data;


				vector<triangle_face> face_list;


				// Copy Listen anlegen
				vector<GLfloat> copy_vData;
				vector<GLfloat> copy_nData;
				vector<GLfloat> copy_uvData;

				vector<GLfloat> copy_tData;
				vector<GLfloat> copy_bData;


				ifstream input(modelpath);

				if(!input)
				{
					std::cout << "Model not found!" << std::endl;
					exit(0xaffe);
				}

				// File laden
				while(!input.eof())
				{
					string line;

					std::getline(input, line);

					// Stirb, nullpointerexception!
					if(line.size() != 0)
					{
						switch(line[0])
						{
							// Kommentar
						case '#':
							continue;
							break;

							// Objekt-Daten
						case 'v':
							switch(line[1])
							{
								// Vertex
							case ' ':
								{
									vec3<float> v_vec;
									sscanf_s(line.c_str(), "v %f %f %f", &v_vec.x, &v_vec.y, &v_vec.z);
									vertex_data.push_back(v_vec);
								}
								break;

								// Texturkoordinaten
							case 't':
								{
									vec2<float> uv_vec;
									sscanf_s(line.c_str(), "vt %f %f", &uv_vec.x, &uv_vec.y);
									uv_data.push_back(uv_vec);
								}
								break;

								// Normalen
							case 'n':
								{
									vec3<float> n_vec;
									sscanf_s(line.c_str(), "vn %f %f %f", &n_vec.x, &n_vec.y, &n_vec.z);
									normal_data.push_back(n_vec);
								}
								break;

							default:
								break;
							}
							break;

							// Face-Daten
						case 'f':
							{
								triangle_face f;
								sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &f.v1.x, &f.v1.y, &f.v1.z, &f.v2.x, &f.v2.y, &f.v2.z, &f.v3.x, &f.v3.y, &f.v3.z);

								f.v1.x -= 1; f.v1.y -= 1; f.v1.z -= 1;
								f.v2.x -= 1; f.v2.y -= 1; f.v2.z -= 1;
								f.v3.x -= 1; f.v3.y -= 1; f.v3.z -= 1;

								face_list.push_back(f);
							}
							break;

						default:
							break;
						}
					}
				}


				vector<vec3<float>> allVertices;
				vector<vec3<float>> allNormals;


				// Nur eine Indexliste in OpenGL möglich -> Vertices müssen dupliziert werden.
				for(unsigned int i = 0; i < face_list.size(); i++)
				{
					triangle_face face = face_list[i];
					
					vec3<float> vert1, vert2, vert3;
					vec3<float> norm1, norm2, norm3;
					vec2<float> uv1, uv2, uv3;
					vec3<float> triangletangent, trianglebitangent;

					// Vertices
					vert1 = vertex_data[face.v1.x];
					vert2 = vertex_data[face.v2.x];
					vert3 = vertex_data[face.v3.x];

					copy_vData.push_back(vert1.x); copy_vData.push_back(vert1.y); copy_vData.push_back(vert1.z);
					copy_vData.push_back(vert2.x); copy_vData.push_back(vert2.y); copy_vData.push_back(vert2.z);
					copy_vData.push_back(vert3.x); copy_vData.push_back(vert3.y); copy_vData.push_back(vert3.z);

					if(heavyLoading)
					{
						allVertices.push_back(vert1);
						allVertices.push_back(vert2);
						allVertices.push_back(vert3);
					}


					// Normalen
					norm1 = normal_data[face.v1.z];
					norm2 = normal_data[face.v2.z];
					norm3 = normal_data[face.v3.z];

					copy_nData.push_back(norm1.x); copy_nData.push_back(norm1.y); copy_nData.push_back(norm1.z);
					copy_nData.push_back(norm2.x); copy_nData.push_back(norm2.y); copy_nData.push_back(norm2.z);
					copy_nData.push_back(norm3.x); copy_nData.push_back(norm3.y); copy_nData.push_back(norm3.z);

					if(heavyLoading)
					{
						allNormals.push_back(norm1);
						allNormals.push_back(norm2);
						allNormals.push_back(norm3);
					}


					// UVs
					uv1 = uv_data[face.v1.y];
					uv2 = uv_data[face.v2.y];
					uv3 = uv_data[face.v3.y];

					copy_uvData.push_back(uv1.x); copy_uvData.push_back(uv1.y);
					copy_uvData.push_back(uv2.x); copy_uvData.push_back(uv2.y);
					copy_uvData.push_back(uv3.x); copy_uvData.push_back(uv3.y);

					if(heavyLoading)
					{
						glm::vec3 q1, q2;

						q1 = glm::vec3(vert2.x, vert2.y, vert2.z) - glm::vec3(vert1.x, vert1.y, vert1.z);
						q2 = glm::vec3(vert3.x, vert3.y, vert3.z) - glm::vec3(vert1.x, vert1.y, vert1.z);

						glm::vec2 st1, st2;

						st1 = glm::vec2(uv2.x, uv2.y) - glm::vec2(uv1.x, uv1.y);
						st2 = glm::vec2(uv3.x, uv3.y) - glm::vec2(uv1.x, uv1.y);

						float divider = (st1.x * st2.y - st2.x * st1.y);

						if(!(divider != 0.f))
							divider = std::numeric_limits<float>::epsilon();

						float coeff = 1 / divider;

						float stmat[4];

						stmat[0] = st2.y * coeff;		stmat[1] = -st1.y * coeff;
						stmat[2] = -st2.x * coeff;	stmat[3] = st1.x * coeff;

						float qmat[6];

						qmat[0] = q1.x;		qmat[1] = q1.y;		qmat[2] = q1.z;
						qmat[3] = q2.x;		qmat[4] = q2.y;		qmat[5] = q2.z;

						triangletangent.x = stmat[0] * qmat[0] + stmat[1] * qmat[3];
						triangletangent.y = stmat[0] * qmat[1] + stmat[1] * qmat[4];
						triangletangent.z = stmat[0] * qmat[2] + stmat[1] * qmat[5];

						tangent_data.push_back(triangletangent);
						tangent_data.push_back(triangletangent);
						tangent_data.push_back(triangletangent);

						trianglebitangent.x = stmat[2] * qmat[0] + stmat[3] * qmat[3];
						trianglebitangent.y = stmat[2] * qmat[1] + stmat[3] * qmat[4];
						trianglebitangent.z = stmat[2] * qmat[2] + stmat[3] * qmat[5];

						bitangent_data.push_back(trianglebitangent);
						bitangent_data.push_back(trianglebitangent);
						bitangent_data.push_back(trianglebitangent);
					}
				}

				if(heavyLoading)
				{
					int totalVerts = allVertices.size();

					// Tangenten und Bitangenten mitteln
					for(int i = 0; i < totalVerts; i++)
					{
						std::vector<int> indices;

						vec3<float> base = allVertices[i];

						// Falls am aktuellen Ort ein Vertex -> Index merken, sonst zum nächsten Ort
						if(!base.reached)
							indices.push_back(i);

						else
							continue;


						int remainingVerts = i + 1;


						for(remainingVerts; remainingVerts < totalVerts; remainingVerts++)
						{
							vec3<float> further = allVertices[remainingVerts];

							if(glm::vec3(base.x, base.y, base.z) == glm::vec3(further.x, further.y, further.z))
								indices.push_back(remainingVerts);
						}

						glm::vec3 avgTangent, avgBitangent;

						for(unsigned int found_i = 0; found_i < indices.size(); found_i++)
						{
							avgTangent += glm::vec3(tangent_data[indices[found_i]].x, tangent_data[indices[found_i]].y, tangent_data[indices[found_i]].z);
							avgBitangent += glm::vec3(bitangent_data[indices[found_i]].x, bitangent_data[indices[found_i]].y, bitangent_data[indices[found_i]].z);
						}

						avgTangent = glm::normalize(avgTangent);
						avgBitangent = glm::normalize(avgBitangent);

						for(unsigned int found_i = 0; found_i < indices.size(); found_i++)
						{
							tangent_data[indices[found_i]].x = avgTangent.x;
							tangent_data[indices[found_i]].y = avgTangent.y;
							tangent_data[indices[found_i]].z = avgTangent.z;
							bitangent_data[indices[found_i]].x = avgBitangent.x;
							bitangent_data[indices[found_i]].y = avgBitangent.y;
							bitangent_data[indices[found_i]].z = avgBitangent.z;

							allVertices[indices[found_i]].reached = true;
						}
					}

					// Orthogonalisieren und ausrichten
					for(int a = 0; a < totalVerts; a++)
					{
						glm::vec3 tangent(tangent_data[a].x, tangent_data[a].y, tangent_data[a].z);
						glm::vec3 bitangent(bitangent_data[a].x, bitangent_data[a].y, bitangent_data[a].z);
						glm::vec3 normal(allNormals[a].x, allNormals[a].y, allNormals[a].z);

						tangent = (tangent - normal) * glm::dot(normal, tangent);
						bitangent = glm::cross(normal, tangent);

						tangent = glm::normalize(tangent);
						bitangent = glm::normalize(bitangent);

						glm::vec3 cross = glm::cross(normal, tangent);

						if(glm::dot(cross, bitangent) < 0.0f)
							tangent *= -1.0f;

						tangent_data[a].x = tangent.x;
						tangent_data[a].y = tangent.y;
						tangent_data[a].z = tangent.z;
						bitangent_data[a].x = bitangent.x;
						bitangent_data[a].y = bitangent.y;
						bitangent_data[a].z = bitangent.z;
					}

					copy_tData.reserve(3 * tangent_data.size());
					copy_bData.reserve(3 * bitangent_data.size());

					for(unsigned int i = 0; i < tangent_data.size(); i++)
					{
						copy_tData.push_back(tangent_data[i].x);	copy_tData.push_back(tangent_data[i].y);	copy_tData.push_back(tangent_data[i].z);
						copy_bData.push_back(bitangent_data[i].x);	copy_bData.push_back(bitangent_data[i].y);	copy_bData.push_back(bitangent_data[i].z);
					}
				}


				faces = face_list.size();


				vertices = copy_vData;
				normals = copy_nData;
				uvs = copy_uvData;

				tangents = copy_tData;
				bitangents = copy_bData;
			}
			else
			{
				std::cout << "Model: " << modelpath << " nicht im .obj oder .binobj Format" << std::endl;
				exit(0xaffe);
			}
		}



		OBJModel::~OBJModel(void)
		{
			// Nothing to do, smart ptr
		}


		void OBJModel::writeOutBinary(const std::string& path)
		{
			std::ofstream output(path, std::ios::out | std::ios::binary);

			if(!output)
			{
				std::cout << "Datei: " << path << " nicht gefunden!" << std::endl;
				exit(0xabab);
			}

			nData ndata;

			ndata.bitangents = bitangents.size();
			ndata.faces = faces;
			ndata.normals = normals.size();
			ndata.tangents = tangents.size();
			ndata.uvs = uvs.size();
			ndata.vertices = vertices.size();

			output.write((char *) &ndata, sizeof(nData));

			long offset = sizeof(nData);

			output.seekp(offset, std::ios::beg);
			output.write((char *) vertices.data(), vertices.size() * sizeof(float));

			offset += vertices.size() * sizeof(float);

			output.seekp(offset, std::ios::beg);
			output.write((char *) uvs.data(), uvs.size() * sizeof(float));

			offset += uvs.size() * sizeof(float);

			output.seekp(offset, std::ios::beg);
			output.write((char *) normals.data(), normals.size() * sizeof(float));

			if(heavyLoading)
			{
				offset += normals.size() * sizeof(float);

				output.seekp(offset, std::ios::beg);
				output.write((char *) tangents.data(), tangents.size() * sizeof(float));

				offset += tangents.size() * sizeof(float);

				output.seekp(offset, std::ios::beg);
				output.write((char *) bitangents.data(), bitangents.size() * sizeof(float));
			}

			output.close();
		}
		//	===================================================================================================================================================================================		#####
	}
}



namespace mgraphics
{
	namespace objmodel
	{
		// IndexedModel																																										 IndexedModel
		//	===================================================================================================================================================================================		#####
		IndexedOBJModel::IndexedOBJModel(std::string path)
		{
			std::cout << "Coming Soon" << std::endl;
		}


		IndexedOBJModel::IndexedOBJModel(std::string path, bool withTB)
		{

		}

		IndexedOBJModel::~IndexedOBJModel()
		{

		}
		//	===================================================================================================================================================================================		#####




		// IndexedModelStorage																																						  IndexedModelStorage
		//	===================================================================================================================================================================================		#####
		IndexedModelStorage::IndexedModelStorage(const std::shared_ptr<IndexedOBJModel> model)
		{
			this->m = model;
		}

		void IndexedModelStorage::StoreAsBinary()
		{

		}
		//	===================================================================================================================================================================================		#####
	}
}
//	===========================================================================================================================================================================================		#####
