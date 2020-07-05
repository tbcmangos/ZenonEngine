﻿#include "stdafx.h"

// General
#include "MainEditor.h"

#include <qfilesystemmodel.h>

MainEditor::MainEditor(QWidget* Parent)
	: QMainWindow(Parent)
{
	m_UI.setupUi(this);

	QFileSystemModel* fsModel = new QFileSystemModel(this);
	fsModel->setRootPath("D:\\_programming\\ZenonEngine\\gamedata");
	m_UI.FSTreeViewer->setModel(fsModel);

	m_PropertiesController = std::make_shared<CPropertiesController>(m_UI.PropertyEditor);

	// Unite file browser and log docker
	QMainWindow::tabifyDockWidget(m_UI.DockerFileBrowser, m_UI.DockerLogViewer);

	m_MoverValues.insert(std::make_pair("<disabled>", 0.001f));
	m_MoverValues.insert(std::make_pair("x1.0", 1.0f));
	m_MoverValues.insert(std::make_pair("x5.0", 5.0f));
	m_MoverValues.insert(std::make_pair("x10.0", 10.0f));

	// Add items to Combo Box
	for (const auto& v : m_MoverValues)
		m_UI.Editor3DFrame_MoverStep->addItem(v.first.c_str());
	connect(m_UI.Editor3DFrame_MoverStep, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(MoverStepCurrentIndexChanged(const QString&)));
}

MainEditor::~MainEditor()
{
}


void MainEditor::SetEditor3D(IEditor3DFrame* Editor3DFrame)
{
	m_Editor3D = Editor3DFrame;
	getMainEditor()->SetEditors(Editor3DFrame, this);
	getSceneViewer()->SetEditors(Editor3DFrame, this);
	getCollectionViewer()->SetEditors(Editor3DFrame, this);
	Selector_SetOtherSelector(dynamic_cast<CSceneNodesSelector*>(m_Editor3D));

	std::vector<std::string> realNames;

	auto fileNames = Utils::GetAllFilesInDirectory("C:\\_engine\\ZenonEngine_gamedata\\models", ".fbx");
	for (const auto& it : fileNames)
	{
		Log::Info(it.c_str());

		try
		{
			std::string zenonFileName = it + ".znmdl";
			if (m_Editor3D->GetBaseManager2().GetManager<IFilesManager>()->IsFileExists(zenonFileName))
			{
				realNames.push_back(zenonFileName);
				continue;
			}

			std::shared_ptr<ISceneNode3D> sceneNode = m_Editor3D->GetBaseManager2().GetManager<IObjectsFactory>()->GetClassFactoryCast<ISceneNode3DFactory>(ofkSceneNode3D)->CreateSceneNode3D(dynamic_cast<IScene*>(m_Editor3D), cSceneNode_FBXNode);

			std::shared_ptr<IFBXSceneNode3D> fbxSceneNode = std::dynamic_pointer_cast<IFBXSceneNode3D>(sceneNode);
			fbxSceneNode->InitializeFromFile(it);

			auto model = std::dynamic_pointer_cast<IFBXSceneNode3D>(sceneNode->GetChilds().at(0))->GetModel();
			if (auto loadable = std::dynamic_pointer_cast<IObjectLoadSave>(model))
			{
				std::shared_ptr<IFile> file = std::make_shared<CFile>(zenonFileName);
				loadable->Save(file);

				m_Editor3D->GetBaseManager2().GetManager<IFilesManager>()->GetFilesStorage("PCEveryFileAccess")->SaveFile(file);
				realNames.push_back(zenonFileName);
			}
		}
		catch (const CException& e)
		{
			Log::Error(e.MessageCStr());
		}
	}

	getCollectionViewer()->SetModelsList(realNames);
}


namespace
{
	void FillActionsList(const std::shared_ptr<ISceneNode3D>& Node, std::map<std::string, std::vector<std::shared_ptr<IPropertyAction>>> * actionsNames)
	{
		for (const auto& it : Node->GetProperties()->GetProperties())
		{
			if (auto act = std::dynamic_pointer_cast<IPropertyAction>(it.second))
			{
				std::vector<std::shared_ptr<IPropertyAction>> actions;
				actions.push_back(act);
				actionsNames->insert(std::make_pair(it.second->GetName(), actions));
			}
		}
	}

	void FillActionsMap(const std::shared_ptr<ISceneNode3D>& Node, std::map<std::string, std::shared_ptr<IPropertyAction>> * actionsNames)
	{
		for (const auto& it : Node->GetProperties()->GetProperties())
		{
			if (auto act = std::dynamic_pointer_cast<IPropertyAction>(it.second))
			{
				actionsNames->insert(std::make_pair(it.second->GetName(), act));
			}
		}
	}
}


//
// IEditorUIFrame
//
void MainEditor::ExtendContextMenu(QMenu * Menu, const std::shared_ptr<ISceneNode3D>& Node)
{
	std::string contextMenuTitle = Node->GetName();

	std::vector<QAction *> actions;

	if (Selector_GetSelectedNodes().size() > 1)
	{
		contextMenuTitle.append("and " + std::to_string(Selector_GetSelectedNodes().size()) + " nodes.");

		std::map<std::string, std::vector<std::shared_ptr<IPropertyAction>>> mainNodeActionsNames;
		FillActionsList(Node, &mainNodeActionsNames);

		for (const auto& sel : Selector_GetSelectedNodes())
		{
			if (sel == Node)
				continue;

			std::map<std::string, std::shared_ptr<IPropertyAction>> selNodeActionsNames;
			FillActionsMap(sel, &selNodeActionsNames);

			for (const auto& act : selNodeActionsNames)
			{
				auto it = std::find_if(mainNodeActionsNames.begin(), mainNodeActionsNames.end(), [&act](const std::pair<std::string, std::vector<std::shared_ptr<IPropertyAction>>>& mainNodeAction) -> bool {
					return mainNodeAction.first == act.first;
				});

				if (it == mainNodeActionsNames.end())
					continue;

				mainNodeActionsNames[act.first].push_back(act.second);
			}
		}

		for (const auto& it : mainNodeActionsNames)
		{
			std::string actionName = it.first;
			auto actionsList = it.second;
			if (actionsList.size() > 1)
				actionName.append(" [" + std::to_string(actionsList.size()) + "]");
			QAction * action = new QAction(actionName.c_str(), this);
			this->connect(action, &QAction::triggered, this, [actionsList] {
				for (const auto& act : actionsList)
					act->ExecuteAction();
			});
			actions.push_back(action);
		}
	}
	else
	{
		for (const auto& it : Node->GetProperties()->GetProperties())
		{
			if (auto act = std::dynamic_pointer_cast<IPropertyAction>(it.second))
			{
				QAction * action = new QAction(it.second->GetName().c_str(), this);
				connect(action, &QAction::triggered, this, [act] {
					act->ExecuteAction();
				});
				actions.push_back(action);
			}
		}

	}

	/* Create actions to the context menu */
	QAction* nameAction = new QAction(contextMenuTitle.c_str(), this);
	nameAction->setEnabled(false);

	/* Set the actions to the menu */
	Menu->addAction(nameAction);
	Menu->addSeparator();
	for (const auto& it : actions)
		Menu->addAction(it);
}

void MainEditor::OnSceneChanged()
{
	getSceneViewer()->RefreshTreeViewModel();
}



//
// CSceneNodesSelector
//
void MainEditor::Selector_OnSelectionChange()
{
	const auto& selectedNodes = Selector_GetSelectedNodes();
	if (selectedNodes.size() == 1)
		getSceneViewer()->SelectNode(Selector_GetFirstSelectedNode());

	getSceneViewer()->SelectNodes(selectedNodes);

	m_PropertiesController->OnSceneNodeSelected(Selector_GetFirstSelectedNode().get());
}


//
// Signals
//
void MainEditor::MoverStepCurrentIndexChanged(const QString & String)
{
	auto it = m_MoverValues.find(String.toStdString());
	if (it == m_MoverValues.end())
		_ASSERT(FALSE);

	m_Editor3D->SetMoverValue(it->second);
}
