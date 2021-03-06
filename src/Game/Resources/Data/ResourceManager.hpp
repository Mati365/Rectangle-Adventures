/*
 * ResourceManager.hpp
 *
 *  Created on: 18-05-2013
 *      Author: mateusz
 */

#ifndef RESOURCEMANAGER_HPP_
#define RESOURCEMANAGER_HPP_
#include <stdio.h>

#include "../../Tools/Tools.hpp"
#include "../../Tools/Logger.hpp"
#include "../../Tools/Converter.hpp"

/**
 * TO JEST OSOBNA CZESC NIE POWIAZANA
 * Z SYSTEMEM PLIKOW!
 * ------------------------
 * Menedzer zasobow przechowuje modele itp.
 * i zarzadza nimi
 */
class Resource {
	protected:
		char* label;
		usint resource_id;

	public:
		Resource(const char* _label) :
						label(Convert::getDynamicValue(_label)),
						resource_id(0) {
		}
		
		virtual bool load(FILE*) = 0;
		virtual void unload() = 0;

		/**
		 * Nadawanie ID podczas
		 * dodawanie obiektu!
		 */
		void setResourceID(usint _id) {
			resource_id = _id;
		}
		
		usint getResourceID() const {
			return resource_id;
		}
		
		const char* getLabel() {
			return label;
		}
		
		virtual ~Resource() {
			if (label) {
				delete[] label;
			}
		}
};

class ResourceManager {
	private:
		/** 1 argument - group ID */
		deque<Resource*> resources;
		usint counter;

	public:
		ResourceManager() :
						counter(1) {
		}
		
		/** Zwracanie identyfikatoru */
		usint addResource(Resource* _res) {
			resources.push_back(_res);
			_res->setResourceID(counter++);
			
			return counter;
		}
		
		/** Zwracanie identyfikatora */
		usint getID(const char* _str) {
			Resource* res = getByLabel(_str);
			if (res) {
				return res->getResourceID();
			}
			return 0;
		}
		
		/** Zwracanie calego obiektu po label'u! */
		Resource* getByLabel(const char* _str) {
			for (auto& res : resources) {
				if (strcmp(res->getLabel(), _str) == 0) {
					return res;
				}
			}
			return nullptr;
		}
		
		/** Pobieranie elementow */
		Resource* operator[](usint _id) {
			return getByID(_id);
		}
		
		Resource* getByID(usint _id) {
			for (auto& res : resources) {
				if (res->getResourceID() == _id) {
					return res;
				}
			}
			return nullptr;
		}
		
		/** Kasowanie zasobu - wolne */
		bool deleteResource(usint _id) {
			for (usint i = 0; i < resources.size(); ++i) {
				Resource* res = resources[i];
				if (res->getResourceID() == _id) {
					safe_delete(res);
					resources.erase(resources.begin() + i);
					return true;
				}
			}
			return false;
		}
		
		/** Kasowanie niedobitkow */
		~ResourceManager() {
			usint count = 0;
			for (auto& obj : resources) {
				if (!obj) {
					continue;
				}
				safe_delete<Resource>(obj);
				count++;
			}
			logEvent(
					Logger::LOG_INFO,
					"Usunięto " + Convert::toString<usint>(count)
							+ " niedobitków!");
		}
};

#endif /* RESOURCEMANAGER_HPP_ */
