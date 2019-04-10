#include "CLevelGeneric.h"
#include "CCameraManager.h"
#include "CGameManager.h"

#include "Theora/TheoraVideoManager.h"
#include "Theora/TheoraVideoClip.h"

using namespace Core;

#define VIDEO_FILE "konqi.ogg"

TheoraVideoClip* getClip(String name)
{
	TheoraVideoManager* mgr = TheoraVideoManager::getSingletonPtr();
	return mgr->getVideoClipByName(name);
}

void createQuad(SceneManager* SceneMgr, String name,String material_name,float left,float top,float right,float bottom)
{
	ManualObject* model = SceneMgr->createManualObject(name);
	model->begin(material_name);

	model->position(right,bottom,0); model->textureCoord(1,1);
	model->position(right,top   ,0); model->textureCoord(1,0);
	model->position(left ,top   ,0); model->textureCoord(0,0);
	model->position(left ,top   ,0); model->textureCoord(0,0);
	model->position(right,bottom,0); model->textureCoord(1,1);
	model->position(left, bottom,0); model->textureCoord(0,1);

	model->end();
	// make the model 2D
	model->setUseIdentityProjection(true);
	model->setUseIdentityView(true);
	// and atach it to the root node
	SceneNode* node = SceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(model);
}

CLevelGeneric::CLevelGeneric()
{
}

CLevelGeneric::~CLevelGeneric()
{
}

void CLevelGeneric::Update(const f32& elapsedTime)
{
	TheoraVideoManager* mgr = TheoraVideoManager::getSingletonPtr();
	TheoraVideoClip* clip=mgr->getVideoClipByName(VIDEO_FILE);
	float dur=clip->getDuration();
	String s=StringConverter::toString(dur);
	String s2=StringConverter::toString(clip->getTimePosition(),4);

	float cTime=clip->getTimePosition();
	float duration=clip->getDuration();
	int pos=1024*(cTime/duration);

	//TheoraVideoClip* clip=getClip(VIDEO_FILE);
	//float dur=clip->getDuration();

	//int step=StringConverter::parseInt(prop.c_str());
	//if (abs(step-mSeekStep) > 10)
	//{
	//	mSeekStep=step;
	//	float seek_time=((float) step/1024)*dur;

	//	clip->seek(seek_time);
	//}
}

void CLevelGeneric::End()
{
	m_GameManager->RemoveResourceArchive(".\\Media.zip", "CLevelGeneric");
}

bool CLevelGeneric::Load()
{
	m_GameManager->AddResourceArchive(".\\Media.zip", "CLevelGeneric");
	m_SceneManager->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

	// Initialize video
	createQuad(m_SceneManager, "video_quad","video_material",-0.5,1,1,-0.94);
	TheoraVideoManager* mgr=TheoraVideoManager::getSingletonPtr();
	mgr->setInputName(VIDEO_FILE);
	mgr->createDefinedTexture("video_material");

    // Position the camera
	CCameraManager::Instance()->CreateCamera(ECM_FREE);
    CCameraManager::Instance()->GetCamera()->setPosition(0, 0, 500);
    CCameraManager::Instance()->GetCamera()->lookAt(0, 0, -300);

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