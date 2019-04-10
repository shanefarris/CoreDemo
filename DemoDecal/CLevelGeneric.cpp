#include "CLevelGeneric.h"
#include "CPhysicsManager.h"
#include "CPhysicsStrategy_Ode.h"
#include "CCameraManager.h"
#include "CLoadingScreen.h"
#include "CSceneImporter.h"
#include "CTextOutput.h"
#include "CItemsManager.h"
#include "CGameObjectManager.h"

using namespace Core;
using namespace Core::Physics;

SceneNode *mProjectorNode;
Frustum *mDecalFrustum;
float mAnim;
Frustum* mFilterFrustum;

void makeMaterialReceiveDecal(const String &matName)
{
	// get the material
	MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().getByName(matName);

	// create a new pass in the material to render the decal
	Pass *pass = mat->getTechnique(0)->createPass();

	// set our pass to blend the decal over the model's regular texture
	pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	pass->setDepthBias(1);

	// set the decal to be self illuminated instead of lit by scene lighting
	pass->setLightingEnabled(false);

	// set up the decal's texture unit
	TextureUnitState *texState = pass->createTextureUnitState("decal.png");
	texState->setProjectiveTexturing(true, mDecalFrustum);
	texState->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	texState->setTextureFiltering(FO_POINT, FO_LINEAR, FO_NONE);

	// set up the filter texture's texture unit
	texState = pass->createTextureUnitState("decal_filter.png");
	texState->setProjectiveTexturing(true, mFilterFrustum);
	texState->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	texState->setTextureFiltering(TFO_NONE);
}

CLevelGeneric::CLevelGeneric() : CGameState()
{
}

CLevelGeneric::~CLevelGeneric()
{
}

void CLevelGeneric::Update(const f32& elapsedTime)
{
	CCameraManager::Instance()->Update(elapsedTime);

	// rotate projector about Y at 10 degrees per second
	mProjectorNode->rotate(Vector3::UNIT_Y, Degree(elapsedTime * 10));
 
     // 2 second animation loop
     mAnim += elapsedTime / 2;
     if (mAnim >= 1)
         mAnim -= 1;

     // animate the FOV of the decal frustum between 5 and 25 degrees
     mDecalFrustum->setFOVy(Degree(15 + Math::Sin(mAnim * Math::TWO_PI) * 10));
}

void CLevelGeneric::End()
{
}

bool CLevelGeneric::Load()
{
	CGameManager::Instance()->AddResourceArchive("./Media.zip", "CLevelGeneric");
	SceneManager* mSceneMgr = CGameManager::Instance()->GetSceneManager();
	// Set ambient light
	mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));
	// Create a light
	Light* l = mSceneMgr->createLight("MainLight");
	l->setPosition(20,80,50);

	// setup player
	CCameraManager::Instance()->CreateCamera(ECM_FREE);
	//CCameraManager::Instance()->GetCamera()->setPosition(0, 100, 300);

	// set up the main decal projection frustum
	mDecalFrustum = new Frustum();
	mProjectorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("DecalProjectorNode");
	mProjectorNode->attachObject(mDecalFrustum);
	mProjectorNode->setPosition(0,5,0);

	// include these two lines if you don't want perspective projection
	//mDecalFrustum->setProjectionType(PT_ORTHOGRAPHIC);
	//mDecalFrustum->setNearClipDistance(25);

	// set up the perpendicular filter texture frustum
	mFilterFrustum = new Frustum();
	mFilterFrustum->setProjectionType(PT_ORTHOGRAPHIC);
	SceneNode *filterNode = mProjectorNode->createChildSceneNode("DecalFilterNode");
	filterNode->attachObject(mFilterFrustum);
	filterNode->setOrientation(Quaternion(Degree(90),Vector3::UNIT_Y));

	Entity *ent;
	for (int i = 0; i < 6; i++)
	{
		SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = mSceneMgr->createEntity("head" + StringConverter::toString(i), "robot.mesh");
		headNode->attachObject(ent);
		Radian angle(i * Math::TWO_PI / 6);
		headNode->setPosition(75 * Math::Cos(angle), 0, 75 * Math::Sin(angle));
		headNode->setScale(2,.5,2);
	}

	// load the images for the decal and the filter
	//TextureManager::getSingleton().load("decal.png", "General", TEX_TYPE_2D, 1);
	//TextureManager::getSingleton().load("decal_filter.png", "General", TEX_TYPE_1D, 1);

	// Make all of the materials in the head entities receive the decal
	for (unsigned int i = 0; i < ent->getNumSubEntities(); i++)
		makeMaterialReceiveDecal(ent->getSubEntity(i)->getMaterialName());

	// Setup FrameListener
	m_FrameListenerFramework = new CFpsFrameListener(m_GameManager->GetRenderWindow());
	m_GameManager->GetEngineDevice()->addFrameListener(m_FrameListenerFramework);

	return true;
}

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "CGame.h"

int main(int argc, char **argv);
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT argc) { return main(1, &strCmdLine); }

int main(int argc, char **argv)
{
	Core::CGame game;
    try 
	{
		game.AddGameState(new CLevelGeneric());
		game.Run();
    } 
	catch( Ogre::Exception& e ) 
	{
		fprintf(stderr, "An exception has occurred: %s\n", e.what());
    }
    
    return 0;
}