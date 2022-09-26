#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API FileReader 
	{
	private:
		std::ifstream m_FileInputStream;

	public:
		FileReader(const std::string& filePath) { m_FileInputStream.open(filePath); }

		~FileReader() { m_FileInputStream.close(); }

		std::string NextLine() 
		{
			std::string fd;
			std::getline(m_FileInputStream, fd);
			return fd;
		}

		bool HasNextLine() { return m_FileInputStream.eof(); }
		
	};
}