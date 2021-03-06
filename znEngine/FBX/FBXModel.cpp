#include "stdafx.h"

#ifdef ZN_FBX_SDK_ENABLE

// General
#include "FBXModel.h"

// Additional
#include "FBXDisplayCommon.h"


inline glm::vec3 ToGLMVec3(const FbxPropertyT<FbxDouble3>& FBXVec3)
{
	return glm::vec3(FBXVec3.Get()[0], FBXVec3.Get()[1], FBXVec3.Get()[2]);
}


CFBXModel::CFBXModel(const IBaseManager& BaseManager, const IFBXNode& FBXNode)
	: ModelProxie(BaseManager.GetApplication().GetRenderDevice().GetObjectsFactory().CreateModel())
	, m_BaseManager(BaseManager)
	, m_FBXNode(FBXNode)
{
}

CFBXModel::~CFBXModel()
{
}

bool CFBXModel::Load(fbxsdk::FbxMesh* NativeMesh)
{
	if (NativeMesh == nullptr)
		return false;

	SetName(NativeMesh->GetName());

	FbxVector4* lControlPoints = NativeMesh->GetControlPoints();
	if (lControlPoints == nullptr)
	{
		Log::Error("FBXMesh doesn't containt control points.");
		return false;
	}

	NativeMesh->ComputeBBox();
	SetBounds(BoundingBox(ToGLMVec3(NativeMesh->BBoxMin), ToGLMVec3(NativeMesh->BBoxMax)));

	if (!NativeMesh->GenerateNormals(true, false))
	{
		Log::Error("Error while generate normals.");
		return false;
	}

	/*DisplayMetaDataConnections(NativeMesh);
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> binormal;
	std::vector<glm::vec3> tangent;*/

	m_Vertices.clear();
	for (int p = 0; p < NativeMesh->GetPolygonCount(); p++)
	{
		int lPolygonSize = NativeMesh->GetPolygonSize(p);
		if (lPolygonSize > 3)
			lPolygonSize = 3;

		for (int j = 0; j < lPolygonSize; j++)
		{
			FBXVertex v;
			ZeroMemory(&v, sizeof(FBXVertex));
			
			int lControlPointIndex = NativeMesh->GetPolygonVertex(p, j);
			v.controlPointIndex = lControlPointIndex;
			v.pos = glm::vec3(lControlPoints[lControlPointIndex][0], lControlPoints[lControlPointIndex][1], lControlPoints[lControlPointIndex][2]);

			//
			// Textures
			//
			int cnt = NativeMesh->GetElementUVCount();
			if (cnt > 1) cnt = 1;
			for (int l = 0; l < cnt; ++l)
			{
				const fbxsdk::FbxGeometryElementUV* leUV = NativeMesh->GetElementUV(l);

				switch (leUV->GetMappingMode())
				{

				case FbxGeometryElement::eByControlPoint:
				{
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						v.uv = glm::vec2(leUV->GetDirectArray().GetAt(lControlPointIndex)[0], leUV->GetDirectArray().GetAt(lControlPointIndex)[1]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						v.uv = glm::vec2(leUV->GetDirectArray().GetAt(id)[0], leUV->GetDirectArray().GetAt(id)[1]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = NativeMesh->GetTextureUVIndex(p, j);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						v.uv = glm::vec2(leUV->GetDirectArray().GetAt(lTextureUVIndex)[0], leUV->GetDirectArray().GetAt(lTextureUVIndex)[1]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					_ASSERT(false);
					break;

				default:
					_ASSERT(false);
					break;
				}
			}



			//
			// Normals
			//
			cnt = NativeMesh->GetElementNormalCount();
			if (cnt > 1) cnt = 1;
			for (int l = 0; l < cnt; ++l)
			{
				fbxsdk::FbxGeometryElementNormal* elem = NativeMesh->GetElementNormal(l);
				switch (elem->GetMappingMode())
				{

				case FbxGeometryElement::eByControlPoint:
				{
					switch (elem->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						v.normal = glm::vec3(elem->GetDirectArray().GetAt(lControlPointIndex)[0], elem->GetDirectArray().GetAt(lControlPointIndex)[1], elem->GetDirectArray().GetAt(lControlPointIndex)[2]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = elem->GetIndexArray().GetAt(lControlPointIndex);
						v.normal = glm::vec3(elem->GetDirectArray().GetAt(id)[0], elem->GetDirectArray().GetAt(id)[1], elem->GetDirectArray().GetAt(id)[2]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = NativeMesh->GetTextureUVIndex(p, j);
					switch (elem->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						v.normal = glm::vec3(elem->GetDirectArray().GetAt(lTextureUVIndex)[0], elem->GetDirectArray().GetAt(lTextureUVIndex)[1], elem->GetDirectArray().GetAt(lTextureUVIndex)[2]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = elem->GetIndexArray().GetAt(lTextureUVIndex);
						v.normal = glm::vec3(elem->GetDirectArray().GetAt(id)[0], elem->GetDirectArray().GetAt(id)[1], elem->GetDirectArray().GetAt(id)[2]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					_ASSERT(false);
					break;

				default:
					_ASSERT(false);
					break;
				}
			}



			//
			// Binormal
			//
			cnt = NativeMesh->GetElementBinormalCount();
			if (cnt > 1)
				cnt = 1;
			for (int l = 0; l < cnt; ++l)
			{
				fbxsdk::FbxGeometryElementBinormal* elem = NativeMesh->GetElementBinormal(l);
				switch (elem->GetMappingMode())
				{

				case FbxGeometryElement::eByControlPoint:
				{
					switch (elem->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						v.binormal = glm::vec3(elem->GetDirectArray().GetAt(lControlPointIndex)[0], elem->GetDirectArray().GetAt(lControlPointIndex)[1], elem->GetDirectArray().GetAt(lControlPointIndex)[2]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = elem->GetIndexArray().GetAt(lControlPointIndex);
						v.binormal = glm::vec3(elem->GetDirectArray().GetAt(id)[0], elem->GetDirectArray().GetAt(id)[1], elem->GetDirectArray().GetAt(id)[2]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = NativeMesh->GetTextureUVIndex(p, j);
					switch (elem->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						v.binormal = glm::vec3(elem->GetDirectArray().GetAt(lTextureUVIndex)[0], elem->GetDirectArray().GetAt(lTextureUVIndex)[1], elem->GetDirectArray().GetAt(lTextureUVIndex)[2]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					_ASSERT(false);
					break;

				default:
					_ASSERT(false);
					break;
				}
			}



			//
			// Tangent
			//
			cnt = NativeMesh->GetElementTangentCount();
			if (cnt > 1)
				cnt = 1;
			for (int l = 0; l < cnt; ++l)
			{
				fbxsdk::FbxGeometryElementTangent* elem = NativeMesh->GetElementTangent(l);
				switch (elem->GetMappingMode())
				{

				case FbxGeometryElement::eByControlPoint:
				{
					switch (elem->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					{
						v.tangent = glm::vec3(elem->GetDirectArray().GetAt(lControlPointIndex)[0], elem->GetDirectArray().GetAt(lControlPointIndex)[1], elem->GetDirectArray().GetAt(lControlPointIndex)[2]);
					}
					break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = elem->GetIndexArray().GetAt(lControlPointIndex);
						v.tangent = glm::vec3(elem->GetDirectArray().GetAt(id)[0], elem->GetDirectArray().GetAt(id)[1], elem->GetDirectArray().GetAt(id)[2]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = NativeMesh->GetTextureUVIndex(p, j);
					switch (elem->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						v.tangent = glm::vec3(elem->GetDirectArray().GetAt(lTextureUVIndex)[0], elem->GetDirectArray().GetAt(lTextureUVIndex)[1], elem->GetDirectArray().GetAt(lTextureUVIndex)[2]);
					}
					break;
					default:
						_ASSERT(false);
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					_ASSERT(false);
					break;

				default:
					_ASSERT(false);
					break;
				}
			}

			m_Vertices.push_back(v);
		}
	}

	SkeletonLoad(NativeMesh);

	IRenderDevice& renderDevice = m_BaseManager.GetApplication().GetRenderDevice();
	m_Geometry = renderDevice.GetObjectsFactory().CreateGeometry();
	m_Geometry->AddVertexBuffer(BufferBinding("POSITION", 0), renderDevice.GetObjectsFactory().CreateVoidVertexBuffer(m_Vertices.data(), m_Vertices.size(), 0, sizeof(FBXVertex)));
	m_Geometry->AddVertexBuffer(BufferBinding("TEXCOORD", 0), renderDevice.GetObjectsFactory().CreateVoidVertexBuffer(m_Vertices.data(), m_Vertices.size(), 12, sizeof(FBXVertex)));
	m_Geometry->AddVertexBuffer(BufferBinding("NORMAL", 0), renderDevice.GetObjectsFactory().CreateVoidVertexBuffer(m_Vertices.data(), m_Vertices.size(), 12 + 8, sizeof(FBXVertex)));
	m_Geometry->AddVertexBuffer(BufferBinding("BLENDWEIGHT", 0), renderDevice.GetObjectsFactory().CreateVoidVertexBuffer(m_Vertices.data(), m_Vertices.size(), 12 + 8 + 12 + 12 + 12, sizeof(FBXVertex)));
	m_Geometry->AddVertexBuffer(BufferBinding("BLENDINDICES", 0), renderDevice.GetObjectsFactory().CreateVoidVertexBuffer(m_Vertices.data(), m_Vertices.size(), 12 + 8 + 12 + 12 + 12 + 16, sizeof(FBXVertex)));

	MaterialLoad(NativeMesh);

	m_Vertices.clear();

#if 0
	if (!binormal.empty())
		m_Geometry->AddVertexBuffer(BufferBinding("BINORMAL", 0), renderDevice.GetObjectsFactory().CreateVoidVertexBuffer(binormal));

	if (!tangent.empty())
		m_Geometry->AddVertexBuffer(BufferBinding("TANGENT", 0), renderDevice.GetObjectsFactory().CreateVoidVertexBuffer(tangent));
#endif

	Log::Info("CFBXModel: Mesh '%s' loaded. Polygons count = '%d'.", NativeMesh->GetName(), NativeMesh->GetPolygonCount());

	return true;
}



//
// IFBXModel
//
const IFBXNode & CFBXModel::GetOwner() const
{
	return m_FBXNode;
}

std::shared_ptr<IModel> CFBXModel::GetModel()
{
	return shared_from_this();
}



//
// Private
//
void CFBXModel::MaterialLoad(fbxsdk::FbxMesh* NativeMesh)
{
	struct SPolygonConnectionInfo
	{
		int PolygonBegin;
		int PolygonEnd;
	};
	std::unordered_map<int, SPolygonConnectionInfo> polygonConnectionInfos;


	bool lIsAllSame = true;
	for (int l = 0; l < NativeMesh->GetElementMaterialCount(); l++)
	{
		FbxGeometryElementMaterial* lMaterialElement = NativeMesh->GetElementMaterial(l);
		if (lMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
		{
			lIsAllSame = false;
			break;
		}
	}

	if (lIsAllSame)
	{
		_ASSERT(NativeMesh->GetElementMaterialCount() == 1);
		for (int l = 0; l < NativeMesh->GetElementMaterialCount(); l++)
		{
			FbxGeometryElementMaterial* lMaterialElement = NativeMesh->GetElementMaterial(l);
			if (lMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame)
			{
				int lMatId = lMaterialElement->GetIndexArray().GetAt(0);
				_ASSERT(lMatId >= 0);

				//FbxSurfaceMaterial* lMaterial = NativeMesh->GetNode()->GetMaterial(lMatId);
				AddConnection(m_FBXNode.GetFBXMaterial(lMatId)->GetMaterial(), m_Geometry);
			}
			else
			{
				_ASSERT(false);
			}
		}

		//no material
		if (NativeMesh->GetElementMaterialCount() == 0)
			DisplayString("        no material applied");
	}
	else
	{
		for (int polygonIndex = 0; polygonIndex < NativeMesh->GetPolygonCount(); polygonIndex++)
		{
			_ASSERT(NativeMesh->GetElementMaterialCount() == 1);
			for (int l = 0; l < NativeMesh->GetElementMaterialCount(); l++)
			{
				FbxGeometryElementMaterial* lMaterialElement = NativeMesh->GetElementMaterial(l);
				int lMatId = lMaterialElement->GetIndexArray().GetAt(polygonIndex);
				_ASSERT(lMatId >= 0);

				std::unordered_map<int, SPolygonConnectionInfo>::iterator it = polygonConnectionInfos.find(lMatId);
				if (it == polygonConnectionInfos.end())
				{
					polygonConnectionInfos.insert(std::make_pair(lMatId, SPolygonConnectionInfo{ polygonIndex, polygonIndex }));
				}
				else
				{
					(*it).second.PolygonEnd = polygonIndex;
				}
			}
		}

		for (const auto& it : polygonConnectionInfos)
		{
			SGeometryDrawArgs GeometryDrawArgs;
			GeometryDrawArgs.VertexStartLocation = it.second.PolygonBegin * 3;
			GeometryDrawArgs.VertexCnt = it.second.PolygonEnd * 3 - GeometryDrawArgs.VertexStartLocation + 3;

			AddConnection(m_FBXNode.GetFBXMaterial(it.first)->GetMaterial(), m_Geometry, GeometryDrawArgs);

			//Log::Info("Material with id '%d' added for (%d to %d)", it.first, GeometryDrawArgs.VertexStartLocation, GeometryDrawArgs.VertexCnt);
		}
	}
}

void CFBXModel::SkeletonLoad(fbxsdk::FbxMesh* NativeMesh)
{
#if 1
	FbxVector4* controlPoints = NativeMesh->GetControlPoints();
	if (controlPoints == nullptr)
	{
		Log::Warn("FBXMesh: Control points not found for model %s.", NativeMesh->GetName());
		return;
	}

	for (int deformerIndex = 0; deformerIndex < NativeMesh->GetDeformerCount(); ++deformerIndex)
	{
		fbxsdk::FbxSkin* skin = reinterpret_cast<fbxsdk::FbxSkin*>(NativeMesh->GetDeformer(deformerIndex, fbxsdk::FbxDeformer::eSkin));
		if (skin == nullptr)
		{
			Log::Warn("FBXMesh: Skin not found for model %s.", NativeMesh->GetName());
			continue;
		}

		for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); ++clusterIndex)
		{
			fbxsdk::FbxCluster* cluster = skin->GetCluster(clusterIndex);
			std::string jointname = cluster->GetLink()->GetName();

			// Global pose
			fbxsdk::FbxAMatrix transformMatrix;
			cluster->GetTransformMatrix(transformMatrix);

			fbxsdk::FbxAMatrix transformLinkMatrix;
			cluster->GetTransformLinkMatrix(transformLinkMatrix);

			fbxsdk::FbxAMatrix transformAssociateModelMatrix;
			cluster->GetTransformAssociateModelMatrix(transformAssociateModelMatrix);

			fbxsdk::FbxAMatrix globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix;

			glm::mat4 globalBindposeInverseMatrixGLM;
			for (uint32 i = 0; i < 4; i++)
				for (uint32 j = 0; j < 4; j++)
					globalBindposeInverseMatrixGLM[i][j] = globalBindposeInverseMatrix[i][j];

			auto& skeleton = m_FBXNode.GetFBXScene().GetFBXSkeleton()->GetSkeletonEditable();
			size_t jointIndex = skeleton.GetBoneIndexByName(jointname);
			auto& joint = skeleton.GetBoneByNameEditable(jointname);
			//joint.GlobalInverse = globalBindposeInverseMatrixGLM;
			//joint->Node = cluster->GetLink();
			//joint->Mesh = NativeMesh;

			std::map<int, std::vector<std::pair<float, size_t>>> weightIndexes;
			for (int i = 0; i < cluster->GetControlPointIndicesCount(); ++i)
			{
				auto it = weightIndexes.find(i);
				if (it == weightIndexes.end())
				{
					weightIndexes.insert(std::make_pair(i, std::vector<std::pair<float, size_t>>({ std::make_pair(static_cast<float>(cluster->GetControlPointWeights()[i]), jointIndex) })));
					continue;
				}

				it->second.push_back(std::make_pair(static_cast<float>(cluster->GetControlPointWeights()[i]), jointIndex));
			}

			for (const auto& w : weightIndexes)
			{
				for (auto& v : m_Vertices)
				{
					if (v.controlPointIndex == cluster->GetControlPointIndices()[w.first])
					{
						for (size_t i = 0; i < w.second.size(); i++)
						{
							v.indexes[i] = w.second[i].second;
							v.weights[i] = w.second[i].first;
						}
					}
				}
			}
		}
	}
#endif
}

CFBXModel::FBXVertex& CFBXModel::GetVertexByControlPointIndex(int Index)
{
	for (auto& v : m_Vertices)
		if (v.controlPointIndex == Index)
			return v;
	throw CException("Vertex by control point %d not found.", Index);
}

/*void CFBXModel::DisplayMaterialMapping(fbxsdk::FbxGeometryElementMaterial* materialElement)
{
	const char* lMappingTypes[] = { "None", "By Control Point", "By Polygon Vertex", "By Polygon", "By Edge", "All Same" };
	const char* lReferenceMode[] = { "Direct", "Index", "Index to Direct" };


	DisplayString("           Mapping: ", lMappingTypes[materialElement->GetMappingMode()]);
	DisplayString("           ReferenceMode: ", lReferenceMode[materialElement->GetReferenceMode()]);

	if (materialElement->GetReferenceMode() == FbxGeometryElement::eDirect)
	{

	}
	else if (materialElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
	{
		FbxString lString;

		lString = "           Indices: ";

		for (int i = 0; i < materialElement->GetIndexArray().GetCount(); i++)
		{
			lString += materialElement->GetIndexArray().GetAt(i);
			lString += ", ";
		}

		Log::Print(lString);
	}
}*/

#endif
