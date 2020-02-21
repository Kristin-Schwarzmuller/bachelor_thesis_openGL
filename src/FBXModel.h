#pragma once


// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <string>
#include <vector>
//	===========================================================================================================================================================================================		#####




namespace cgbv
{
	namespace fbxmodel
	{
		// Forward Declarations																																						 Forward Declarations
		//	===================================================================================================================================================================================		#####
		class Mesh;
		//	===================================================================================================================================================================================		#####

		// FBXModel Klasse																																								  FBXModel Klasse
		//	===================================================================================================================================================================================		#####
		class FBXModel
		{
			std::vector<Mesh> meshes;

		public:
			FBXModel(std::string modelpath);
			~FBXModel();

			const std::vector<Mesh>& Meshes()
			{
				return meshes;
			}

			int MeshCount()
			{
				return meshes.size();
			}
		};
		//	===================================================================================================================================================================================		#####



		// Mesh Klasse																																										  Mesh Klasse
		//	===================================================================================================================================================================================		#####
		class Mesh
		{
			std::vector<float> vertexData;
			std::vector<float> uvData;
			std::vector<float> normalData;
			std::vector<float> tangentData;
			std::vector<float> bitangentData;

		public:
			Mesh();
			~Mesh();

			const float* VertexData()
			{
				return vertexData.data();
			}

			int VertexCount() const
			{
				return vertexData.size() / 3;
			}

			const float* UVData()
			{
				return uvData.data();
			}

			int UVDataCount() const
			{
				return uvData.size() / 2;
			}

			const float* NormalData()
			{
				return normalData.data();
			}

			int NormalCount() const
			{
				return normalData.size() / 3;
			}

			const float* TangentData()
			{
				return tangentData.data();
			}

			int TangentCount() const
			{
				return tangentData.size() / 3;
			}

			const float* BitangentData()
			{
				return bitangentData.data();
			}

			int BitangentCount() const
			{
				return bitangentData.size() / 3;
			}

			friend class FBXModel;
		};
		//	===================================================================================================================================================================================		#####
	}
}

