#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include <map>
#include "App.h"
#include <iostream>

#define RM ResourceManager::Instance()

class ResourceManager
{
private:
	//Texture Map
	map<string, IDirect3DTexture9*> m_mTextures;
	ResourceManager(){}  //do initialize shit here
	//map pair:  string, IDirect3DTexture9*
public:
	//Texture Map Accessors
	static ResourceManager* Instance()
	{
		static ResourceManager* temp = new ResourceManager();
		return temp;
	}

	void SaveTex(string fName)
	{
		//function for storing a new texture in the map
		//1) Check to see if texture is already there
		if ( m_mTextures.find(fName) == m_mTextures.end())
		{
			//Texture is not in there, load it up
			IDirect3DTexture9* texture;
			D3DXCreateTextureFromFile(gd3dDevice, fName.c_str(), &texture);
			m_mTextures.insert(make_pair(fName, texture));
			cout << "Texture " << fName << " loaded" << endl;
		}else
		{
			//Found the texture already in the array, do not reload
			cout << "Texture " << fName << " already exists" << endl;
			return;
		}
	}

	IDirect3DTexture9* LoadTex(string fName)
	{
		//function for loading and returning a texture from the map
		//1) Run find on the string
		map<string, IDirect3DTexture9*>::iterator it;
		it = m_mTextures.find(fName);

		if ( it == m_mTextures.end())
		{
			//Did not find the texture in the manager
			cout << "Texture " << fName << " does not exist" << endl;
			return NULL;  //just returning null for now... will need error handling
			
		}else
		{
			//Correct texture was found, return pointer to the texture object
			return (*it).second;
		}

	}

	//return the center offset for the sprite file (unscaled)
	D3DXVECTOR3 GetTexCtr(string fName)
	{
		map<string, IDirect3DTexture9*>::iterator it;
		it = m_mTextures.find(fName);

		if ( it == m_mTextures.end())
		{
			//Did not find the texture in the manager
			cout << "Texture " << fName << " does not exist" << endl;
			return D3DXVECTOR3(0, 0, 0);  //just returning null for now... will need error handling
			
		}else
		{
			D3DXIMAGE_INFO tempInfo;
			D3DXGetImageInfoFromFile(fName.c_str(), &tempInfo);
			D3DXVECTOR3 temp;
			temp.y = (tempInfo.Height) / 2.0f;
			temp.x = (tempInfo.Width) / 2.0f;
		}
		
	}

	
	~ResourceManager(){}	//do cleanup stuff here at some point


};

#endif