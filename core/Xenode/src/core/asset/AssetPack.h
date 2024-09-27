#pragma once
#include <Core.h>

namespace Xen
{
	// There are two types of asset packs, a "Runtime" asset pack and A "Editor" pack
	//
	// Runtime Asset pack:
	//	-> All the required assets are divided into 'n' "chunks" of files. The division is based on 
	//	   file size and no single asset needs to be divided into 2 or more files.
	//	-> The "chunks" just contain tightly packed binary data.
	//	-> Since the assets are addressed by AssetHandle, There is an another "metadata" file
	//	   that contains: AssetHandle, The file index(the chunk), the offset and the size of the asset.
	// 
	// Editor Asset pack:
	//	-> Same as a runtime asset pack, but the "chunks" are more text based
	//	-> The "metadata" file should contain more information of a particular asset(eg. its type, and other data).

	struct XEN_API AssetPack
	{
		friend class AssetPackManager;
	private:
		
	};
}