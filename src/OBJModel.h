#pragma once


// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <glad.h>
#include <string>
#include <vector>
#include <memory>
//	===========================================================================================================================================================================================		#####

namespace mgraphics
{
	namespace objmodel
	{
		// OBJModel																																												 OBJModel
		//	===================================================================================================================================================================================		#####
		class OBJModel
		{
		private: 

			std::vector<float> vertices, normals, uvs, tangents, bitangents;

			unsigned int faces;

			bool heavyLoading;

		public:
			OBJModel(const std::string& modelpath, bool withTangentsAndBitangents = true);

			~OBJModel(void);

			// GLfloat-Array mit den float-Werten der Vertices
			const float* getVertices()
			{
				return vertices.data();
			}


			// GLfloat-Array mit den float-Werten der Normalen
			const float* getNormals()
			{
				return normals.data();
			}


			// GLfloat-Array mit den float-Werten der UVs
			const float* getUVs()
			{
				return uvs.data();
			}


			//	Tx	Bx	Nx
			//	Ty	By	Ny
			//	Tz	Bz	Nz
			//
			//	Matrix, um vom Tangentialraum in den Objektraum zu transformieren
			//
			//	Um in den Tangentialraum zu transformieren -> Inverse dieser Matrix
			const float* getTangents()
			{
				if(heavyLoading)
					return tangents.data();

				return nullptr;
			}


			// Kein wirklicher Grund für die Bitangenten hier.
			const float* getBitangents()
			{
				if(heavyLoading)
					return bitangents.data();

				return nullptr;
			}


			// Anzahl der Vertices im Modell
			unsigned int numberVertices()
			{
				return vertices.size() / 3;
			}


			// Anzahl der Normalen im Modell
			unsigned int numberNormals()
			{
				return normals.size() / 3;
			}


			// Anzahl der Texturkoordinaten im Modell
			unsigned int numberUVs()
			{
				return uvs.size() / 2;
			}


			// Anzahl der Faces im Modell
			unsigned int numberFaces()
			{
				return faces;
			}


			unsigned int numberTangents()
			{
				return tangents.size() / 3;
			}


			unsigned int numberBitangents()
			{
				return bitangents.size() / 3;
			}


			// Benötigte float-Buffer-Größe: sizeof(GLfloat) * n_vertices
			unsigned int getVertexBufferSize()
			{
				return vertices.size() * sizeof(float);
			}


			// Benötigte float-Buffer-Größe: sizeof(GLfloat) * n_normals
			unsigned int getNormalBufferSize()
			{
				return normals.size() * sizeof(float);
			}


			// Benötigte float-Buffer-Größe: sizeof(GLfloat) * n_uvs
			unsigned int getUVBufferSize()
			{
				return uvs.size() * sizeof(float);
			}


			unsigned int getTangentBufferSize()
			{
				return tangents.size() * sizeof(float);
			}


			unsigned int getBitangentBufferSize()
			{
				return bitangents.size() * sizeof(float);
			}

			void writeOutBinary(const std::string& path);
		};
		//	===================================================================================================================================================================================		#####




		// IndexedModel																																										 IndexedModel
		//	===================================================================================================================================================================================		#####
		class IndexedOBJModel
		{
			std::vector<float> vertexdata, normaldata, uvdata, tangentdata, bitangentdata;
			std::vector<unsigned int> indices;

			unsigned int faces;

		public:
			IndexedOBJModel(std::string path);
			IndexedOBJModel(std::string path, bool withTB);
			~IndexedOBJModel();

			const float* VertexData()
			{
				return vertexdata.data();
			}
			unsigned int VertexDataSize()
			{
				return vertexdata.size();
			}

			const float* NormalData()
			{
				return normaldata.data();
			}
			unsigned int NormalDataSize()
			{
				return normaldata.size();
			}

			const float* UVData()
			{
				return uvdata.data();
			}
			unsigned int UVDataSize()
			{
				return uvdata.size();
			}

			const float* TangentData()
			{
				return tangentdata.data();
			}
			unsigned int TangentDataSize()
			{
				return tangentdata.size();
			}

			const float* BitangentData()
			{
				return bitangentdata.data();
			}
			unsigned int BitangentDataSize()
			{
				return bitangentdata.size();
			}

			unsigned int* IndexData()
			{
				return indices.data();
			}
			unsigned int IndexDataSize()
			{
				return indices.size();
			}
		};
		//	===================================================================================================================================================================================		#####




		// IndexedModelStorage																																						  IndexedModelStorage
		//	===================================================================================================================================================================================		#####
		class IndexedModelStorage
		{
			std::shared_ptr<IndexedOBJModel> m;

		public:
			IndexedModelStorage(const std::shared_ptr<IndexedOBJModel> model);

			void StoreAsBinary();
		};
		//	===================================================================================================================================================================================		#####
	}
}
