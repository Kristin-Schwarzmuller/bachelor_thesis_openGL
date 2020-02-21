// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include "FBXModel.h"

#include <fbxsdk.h>
#include <iostream>

#include <memory>
//	===========================================================================================================================================================================================		#####




namespace cgbv
{
	namespace fbxmodel
	{
		// Class Declarations																																						   Class Declarations
		//	===================================================================================================================================================================================		#####
		class FMesh
		{
			std::vector<float> vertexData;
			std::vector<float> uvData;
			std::vector<float> normalData;
			std::vector<float> tangentData;
			std::vector<float> bitangentData;

		public:
			FMesh(FbxMesh *fbxmesh);
			~FMesh();

			friend class FBXModel;
		};
		//	===================================================================================================================================================================================		#####


		// FBXModel Klasse																																								  FBXModel Klasse
		//	===================================================================================================================================================================================		#####
		FBXModel::FBXModel(std::string modelPath)
		{
			FbxManager *sdkManager = FbxManager::Create();

			FbxIOSettings *ioSettings = FbxIOSettings::Create(sdkManager, IOSROOT);
			sdkManager->SetIOSettings(ioSettings);

			FbxImporter *fbxImporter = FbxImporter::Create(sdkManager, "");

			if(!(fbxImporter->Initialize(modelPath.c_str(), -1, sdkManager->GetIOSettings())))
			{
				std::cout << "Fehler beim Laden des FBX-Modells: " << modelPath << std::endl;
				return;
			}


			FbxScene *scene = FbxScene::Create(sdkManager, modelPath.c_str());
			fbxImporter->Import(scene);
			fbxImporter->Destroy();


			FbxNode *currentNode = scene->GetRootNode();
			int nodesInScene = currentNode->GetChildCount();


			for(int i = 0; i < nodesInScene; ++i)
			{
				FbxNode *child = currentNode->GetChild(i);

				FbxNodeAttribute *nodeAttribs = child->GetNodeAttribute();

				switch(nodeAttribs->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
				{
					FbxMesh *mesh = child->GetMesh();

					std::unique_ptr<FMesh> fmesh = std::make_unique<FMesh>(mesh);

					Mesh currentMesh;
					currentMesh.vertexData.insert(currentMesh.vertexData.begin(), fmesh->vertexData.data(), fmesh->vertexData.data() + fmesh->vertexData.size());
					currentMesh.uvData.insert(currentMesh.uvData.begin(), fmesh->uvData.data(), fmesh->uvData.data() + fmesh->uvData.size());
					currentMesh.normalData.insert(currentMesh.normalData.begin(), fmesh->normalData.data(), fmesh->normalData.data() + fmesh->normalData.size());
					currentMesh.tangentData.insert(currentMesh.tangentData.begin(), fmesh->tangentData.data(), fmesh->tangentData.data() + fmesh->tangentData.size());
					currentMesh.bitangentData.insert(currentMesh.bitangentData.begin(), fmesh->bitangentData.data(), fmesh->bitangentData.data() + fmesh->bitangentData.size());
					meshes.push_back(currentMesh);
				}
					break;

				case FbxNodeAttribute::eUnknown:
					std::cout << "eUnknown" << std::endl;
					break;

				case FbxNodeAttribute::eNull:
					std::cout << "eNull" << std::endl;
					break;

				case FbxNodeAttribute::eMarker:
					std::cout << "eMarker" << std::endl;
					break;

				case FbxNodeAttribute::eSkeleton:
					std::cout << "eSkeleton" << std::endl;
					break;

				case FbxNodeAttribute::eNurbs:
					std::cout << "eNurbs" << std::endl;
					break;

				case FbxNodeAttribute::ePatch:
					std::cout << "ePatch" << std::endl;
					break;

				case FbxNodeAttribute::eCamera:
					std::cout << "eCamera" << std::endl;
					break;

				case FbxNodeAttribute::eCameraStereo:
					std::cout << "eCameraStereo" << std::endl;
					break;

				case FbxNodeAttribute::eCameraSwitcher:
					std::cout << "eCameraSwitcher" << std::endl;
					break;

				case FbxNodeAttribute::eLight:
					std::cout << "eLight" << std::endl;
					break;

				case FbxNodeAttribute::eOpticalReference:
					std::cout << "eOpticalReference" << std::endl;
					break;

				case FbxNodeAttribute::eOpticalMarker:
					std::cout << "eOpticalMarker" << std::endl;
					break;

				case FbxNodeAttribute::eNurbsCurve:
					std::cout << "eNurbsCurve" << std::endl;
					break;

				case FbxNodeAttribute::eTrimNurbsSurface:
					std::cout << "eTrimNurbsSurface" << std::endl;
					break;

				case FbxNodeAttribute::eBoundary:
					std::cout << "eBoundary" << std::endl;
					break;

				case FbxNodeAttribute::eNurbsSurface:
					std::cout << "eNurbsSurface" << std::endl;
					break;

				case FbxNodeAttribute::eShape:
					std::cout << "eShape" << std::endl;
					break;

				case FbxNodeAttribute::eLODGroup:
					std::cout << "eLODGroup" << std::endl;
					break;

				case FbxNodeAttribute::eSubDiv:
					std::cout << "eSubDiv" << std::endl;
					break;
				}
				//if(nodeAttribs->GetAttributeType() == FbxNodeAttribute::eMesh)
				//{
				//	FbxMesh *mesh = child->GetMesh();

				//	std::unique_ptr<FMesh> fmesh = std::make_unique<FMesh>(mesh);

				//	Mesh currentMesh;
				//	currentMesh.vertexData.insert(currentMesh.vertexData.begin(), fmesh->vertexData.data(), fmesh->vertexData.data() + fmesh->vertexData.size());
				//	currentMesh.uvData.insert(currentMesh.uvData.begin(), fmesh->uvData.data(), fmesh->uvData.data() + fmesh->uvData.size());
				//	currentMesh.normalData.insert(currentMesh.normalData.begin(), fmesh->normalData.data(), fmesh->normalData.data() + fmesh->normalData.size());
				//	currentMesh.tangentData.insert(currentMesh.tangentData.begin(), fmesh->tangentData.data(), fmesh->tangentData.data() + fmesh->tangentData.size());
				//	currentMesh.bitangentData.insert(currentMesh.bitangentData.begin(), fmesh->bitangentData.data(), fmesh->bitangentData.data() + fmesh->bitangentData.size());
				//	meshes.push_back(currentMesh);
				//}
			}

			scene->Destroy(true);
			ioSettings->Destroy();
			sdkManager->Destroy();

			scene = nullptr;
			ioSettings = nullptr;
			sdkManager = nullptr;
		}


		FBXModel::~FBXModel()
		{

		}
		//	===================================================================================================================================================================================		#####




		// Mesh Klasse																																										  Mesh Klasse
		//	===================================================================================================================================================================================		#####
		Mesh::Mesh()
		{

		}

		Mesh::~Mesh()
		{

		}
		//	===================================================================================================================================================================================		#####




		// FMesh Klasse																																										 FMesh Klasse
		//	===================================================================================================================================================================================		#####
		FMesh::FMesh(FbxMesh *fbxmesh)
		{
			int polygonCount = fbxmesh->GetPolygonCount();

			FbxVector4 *controlPoints = fbxmesh->GetControlPoints();

			for(int polygonID = 0, vertexID = 0; polygonID < polygonCount; ++polygonID)
			{
				int verticesInPolygon = fbxmesh->GetPolygonSize(polygonID);

				for(int vertexOfPolygon = 0; vertexOfPolygon < verticesInPolygon; ++vertexOfPolygon, ++vertexID)
				{
					// Vertices
					int polygonVertexIndex = fbxmesh->GetPolygonVertex(polygonID, vertexOfPolygon);
					FbxVector4 currentVertex = controlPoints[polygonVertexIndex];

					vertexData.push_back((float) currentVertex[0]);
					vertexData.push_back((float) currentVertex[1]);
					vertexData.push_back((float) currentVertex[2]);


					// UV Koordinaten
					int uvSets = fbxmesh->GetElementUVCount();
					for(int currentUVSet = 0; currentUVSet < uvSets; ++currentUVSet)
					{
						FbxGeometryElementUV *uvs = fbxmesh->GetElementUV(currentUVSet);
						FbxVector2 currentUV;

						switch(uvs->GetMappingMode())
						{
						case FbxGeometryElement::eByControlPoint:
							switch(uvs->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								currentUV = uvs->GetDirectArray().GetAt(polygonVertexIndex);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int uvID = uvs->GetIndexArray().GetAt(polygonVertexIndex);
								currentUV = uvs->GetDirectArray().GetAt(uvID);
							}
								break;
							default:
								break;
							}
							break;
						case FbxGeometryElement::eByPolygonVertex:
						{
							int uvID = fbxmesh->GetTextureUVIndex(polygonID, vertexOfPolygon);

							switch(uvs->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
							case FbxGeometryElement::eIndexToDirect:
								currentUV = uvs->GetDirectArray().GetAt(uvID);
								break;
							default:
								break;
							}
						}
							break;
						default:
							break;
						}

						uvData.push_back((float) currentUV[0]);
						uvData.push_back((float) currentUV[1]);
					}


					// Normalen
					int normalSets = fbxmesh->GetElementNormalCount();
					for(int currentNormalSet = 0; currentNormalSet < normalSets; ++currentNormalSet)
					{
						FbxGeometryElementNormal *vertexNormals = fbxmesh->GetElementNormal(currentNormalSet);

						switch(vertexNormals->GetMappingMode())
						{
						case FbxGeometryElement::eByPolygonVertex:
						{
							FbxVector4 currentNormal;

							switch(vertexNormals->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								currentNormal = vertexNormals->GetDirectArray().GetAt(vertexID);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int normalID = vertexNormals->GetIndexArray().GetAt(vertexID);
								currentNormal = vertexNormals->GetDirectArray().GetAt(normalID);
							}
								break;
							default:
								break;
							}

							normalData.push_back((float) currentNormal[0]);
							normalData.push_back((float) currentNormal[1]);
							normalData.push_back((float) currentNormal[2]);
						}
							break;
						case FbxGeometryElement::eByPolygon:
							std::cout << "Normals eByPolygon: not implemented yet!" << std::endl;
							break;
						case FbxGeometryElement::eByControlPoint:
						{
							FbxVector4 currentNormal;

							switch(vertexNormals->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								currentNormal = vertexNormals->GetDirectArray().GetAt(polygonVertexIndex);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int normalID = vertexNormals->GetIndexArray().GetAt(polygonVertexIndex);
								currentNormal = vertexNormals->GetDirectArray().GetAt(normalID);
							}
								break;
							default:
								break;
							}

							normalData.push_back((float) currentNormal[0]);
							normalData.push_back((float) currentNormal[1]);
							normalData.push_back((float) currentNormal[2]);
						}
							break;
						default:
							break;
						}
					}


					// Tangentialvektoren
					int tangentSets = fbxmesh->GetElementTangentCount();
					for(int currentTangentSet = 0; currentTangentSet < tangentSets; ++currentTangentSet)
					{
						FbxGeometryElementTangent *vertexTangents = fbxmesh->GetElementTangent(currentTangentSet);

						switch(vertexTangents->GetMappingMode())
						{
						case FbxGeometryElement::eByPolygonVertex:
						{
							FbxVector4 currentTangent;

							switch(vertexTangents->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								currentTangent = vertexTangents->GetDirectArray().GetAt(vertexID);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int normalID = vertexTangents->GetIndexArray().GetAt(vertexID);
								currentTangent = vertexTangents->GetDirectArray().GetAt(normalID);
							}
								break;
							default:
								break;
							}

							tangentData.push_back((float) currentTangent[0]);
							tangentData.push_back((float) currentTangent[1]);
							tangentData.push_back((float) currentTangent[2]);
						}
							break;
						case FbxGeometryElement::eByPolygon:
							std::cout << "Tangents eByPolygon: not implemented yet!" << std::endl;
							break;
						case FbxGeometryElement::eByControlPoint:
						{
							FbxVector4 currentTangent;

							switch(vertexTangents->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								currentTangent = vertexTangents->GetDirectArray().GetAt(polygonVertexIndex);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int normalID = vertexTangents->GetIndexArray().GetAt(polygonVertexIndex);
								currentTangent = vertexTangents->GetDirectArray().GetAt(normalID);
							}
								break;
							default:
								break;
							}

							tangentData.push_back((float) currentTangent[0]);
							tangentData.push_back((float) currentTangent[1]);
							tangentData.push_back((float) currentTangent[2]);
						}
							break;
						default:
							break;
						}
					}


					// Bitangentialvektoren
					int bitangentSets = fbxmesh->GetElementBinormalCount();
					for(int currentBitangentSet = 0; currentBitangentSet < bitangentSets; ++currentBitangentSet)
					{
						FbxGeometryElementBinormal *vertexBitangents = fbxmesh->GetElementBinormal(currentBitangentSet);

						switch(vertexBitangents->GetMappingMode())
						{
						case FbxGeometryElement::eByPolygonVertex:
						{
							FbxVector4 currentBitangent;

							switch(vertexBitangents->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								currentBitangent = vertexBitangents->GetDirectArray().GetAt(vertexID);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int normalID = vertexBitangents->GetIndexArray().GetAt(vertexID);
								currentBitangent = vertexBitangents->GetDirectArray().GetAt(normalID);
							}
								break;
							default:
								break;
							}

							bitangentData.push_back((float) currentBitangent[0]);
							bitangentData.push_back((float) currentBitangent[1]);
							bitangentData.push_back((float) currentBitangent[2]);
						}
							break;
						case FbxGeometryElement::eByPolygon:
							std::cout << "Tangents eByPolygon: not implemented yet!" << std::endl;
							break;
						case FbxGeometryElement::eByControlPoint:
						{
							FbxVector4 currentBitangent;

							switch(vertexBitangents->GetReferenceMode())
							{
							case FbxGeometryElement::eDirect:
								currentBitangent = vertexBitangents->GetDirectArray().GetAt(polygonVertexIndex);
								break;
							case FbxGeometryElement::eIndexToDirect:
							{
								int normalID = vertexBitangents->GetIndexArray().GetAt(polygonVertexIndex);
								currentBitangent = vertexBitangents->GetDirectArray().GetAt(normalID);
							}
								break;
							default:
								break;
							}

							bitangentData.push_back((float) currentBitangent[0]);
							bitangentData.push_back((float) currentBitangent[1]);
							bitangentData.push_back((float) currentBitangent[2]);
						}
							break;
						default:
							break;
						}
					}
				}
			}
		}

		FMesh::~FMesh()
		{

		}
		//	===================================================================================================================================================================================		#####
	}
}