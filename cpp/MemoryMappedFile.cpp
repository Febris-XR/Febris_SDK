// SPDX-FileCopyrightText: 2026 Febris
// SPDX-License-Identifier: Apache-2.0
#include "pch.h"

namespace FebrisCppHelpers
{


	class MemoryMappedFile
	{
	public:
		MemoryMappedFile(const std::string& filePath, std::size_t size) : m_region(nullptr)
		{
			try
			{
				// Create or open the file
				boost::interprocess::file_mapping file(filePath.c_str(), boost::interprocess::read_write);

				// Map the file to memory
				m_region = new boost::interprocess::mapped_region(file, boost::interprocess::read_write, 0, size);
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error mapping file to memory: " << e.what() << std::endl;
			}
		}

		~MemoryMappedFile()
		{
			if (m_region != nullptr)
			{
				delete m_region;
			}
		}

		char* GetData() const
		{
			if (m_region != nullptr)
			{
				return static_cast<char*>(m_region->get_address());
			}

			return nullptr;
		}

	private:
		boost::interprocess::mapped_region* m_region;
	};

	//class MemoryMappedFile {
	//public:
	//	MemoryMappedFile() : shm(open_or_create, "example", read_write), region(shm, read_write) {
	//		// Set the size of the shared memory object
	//		shm.truncate(1024);
	//	};

	//	void writeData(const char* message) {
	//		// Get a pointer to the mapped region
	//		void* addr = region.get_address();

	//		// Write some data to the shared memory object
	//		std::memcpy(addr, message, std::strlen(message));
	//	};

	//	void readData() {
	//		// Get a pointer to the mapped region
	//		void* addr = region.get_address();

	//		// Read the data from the shared memory object
	//		char buffer[1024];
	//		std::memcpy(buffer, addr, region.get_size());

	//		// Print the data to the console
	//		cout << buffer << endl;
	//	};

	//	~MemoryMappedFile() {
	//		// Unmap the mapped region
	//		region.flush();
	//		region.release();

	//		// Close the shared memory object
	//		shm.close();
	//	};

	//private:
	//	shared_memory_object shm;
	//	mapped_region region;
	//};
};