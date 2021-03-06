#include "stdafx.h"

// General
#include "FBXSkeleton.h"

CFBXSkeleton::CFBXSkeleton(const IBaseManager & BaseManager, const IFBXScene& FBXScene)
	: m_BaseManager(BaseManager)
	, m_FBXScene(FBXScene)
{
}

CFBXSkeleton::~CFBXSkeleton()
{
}

void CFBXSkeleton::Load(fbxsdk::FbxScene* FBXScene)
{
	for (int childindex = 0; childindex < FBXScene->GetRootNode()->GetChildCount(); ++childindex)
	{
		FbxNode *node = FBXScene->GetRootNode()->GetChild(childindex);
		ProcessSkeletonHeirarchyre(node, 0, 0, -1);
	}
}

const CSkeleton& CFBXSkeleton::GetSkeleton() const
{
	return m_Skeleton;
}

CSkeleton & CFBXSkeleton::GetSkeletonEditable()
{
	return m_Skeleton;
}



//
// Private
//
void CFBXSkeleton::ProcessSkeletonHeirarchyre(FbxNode * node, int depth, int index, int parentindex)
{
	if (node->GetNodeAttribute() && node->GetNodeAttribute()->GetAttributeType() && node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		CSkeletonBone bone(node->GetName(), parentindex);
		
		fbxsdk::FbxAMatrix globalBindposeInverseMatrix = node->EvaluateGlobalTransform();

		glm::mat4 globalBindposeInverseMatrixGLM;
		for (uint32 i = 0; i < 4; i++)
		{
			for (uint32 j = 0; j < 4; j++)
			{
				globalBindposeInverseMatrixGLM[i][j] = globalBindposeInverseMatrix[i][j];
			}
		}
		bone.GlobalInverse = globalBindposeInverseMatrixGLM;

		m_Skeleton.AddBone(bone);
	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ProcessSkeletonHeirarchyre(node->GetChild(i), depth + 1, m_Skeleton.GetBones().size(), index);
	}
}
