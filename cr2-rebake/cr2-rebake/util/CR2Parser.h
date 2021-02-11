// [rebake project] CR2
// author: SukJoon Oh, acoustikue@yonsei.ac.kr
// '21.02.07.
//
// Modified

#pragma once
#include "../CR2Config.h"

#ifdef CR2

#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>

#include <iostream>

#include "../CR2Type.h"

namespace cr2 {

	class CR2Parser {
		typedef std::vector<cr2::Edge> EdgeList;

	public:
		CR2Parser(std::string edgeListFileName) {
			this->edgeListFileName = edgeListFileName;
		}

		void readFiles() {
			// read edge file
			uint32_t src, dest;

			std::ifstream edgeFile(edgeListFileName);

			if (!edgeFile.is_open()) {
				std::cout << "cr2::CR2Parser:\n    file open fail: " << edgeListFileName << std::endl;
				std::exit(-1);
			}

			while (edgeFile >> src >> dest) {
				if (src != dest)
					edgeList.push_back(Edge(src, dest));
			}
			std::cout << "cr2::CR2Parser:\n    finish reading" << std::endl;
		}

		const EdgeList getEdgeList() { return edgeList; }

	private:
		std::string edgeListFileName;

		EdgeList edgeList;
	};
}

#endif
