/*
 * Files.hpp
 *
 *  Created on: 20-06-2013
 *      Author: mateusz
 */

#ifndef FILES_HPP_
#define FILES_HPP_
#include "Filesystem.hpp"

using namespace Filesystem;

namespace File {
	/** Import/export tekstu! */
	class Text: public FilePackage {
		public:
			char* text;

			Text(const char* _text) :
							text(const_cast<char*>(_text)) {
			}
			
			Text() :
							text(NULL) {
			}
			
			virtual bool read(FILE* file) {
				if (!file) {
					return false;
				}
				const char* _text = IO::readString(file);
				if (!_text) {
					return false;
				}
				if (text) {
					delete[] text;
				}
				text = new char[strlen(_text)];
				strcpy(text, _text);
				delete[] _text;
				return true;
			}
			
			virtual size_t write(FILE* file) {
				IO::writeString(file, text);
				return getLength();
			}
			
			virtual size_t getLength() {
				if (!text) {
					return 0;
				}
				return IO::stringLength(text);
			}
	};
	/**
	 * Eksport/import plikow!
	 * TUTAJ FUNKCJE KOMPRESUJACE!
	 */
	class ExternalFile: public FilePackage {
#define BUFFER_SIZE 200
			
		protected:
			FILE* file;
			const char* path;

		public:
			ExternalFile() :
							file(NULL),
							path(NULL) {
			}
			
			ExternalFile(const char*);

			virtual bool read(FILE*);
			virtual size_t write(FILE*);

			virtual size_t getLength();

			size_t getFileLength() const {
				return length;
			}
			
			FILE* getFile() const {
				return file;
			}
			
			~ExternalFile() {
				if (file) {
					CLOSE_FILE(file);
				}
			}
	};
	
	bool moveBytes(FILE*, FILE*, size_t, size_t, size_t);
}

#endif /* FILES_HPP_ */
