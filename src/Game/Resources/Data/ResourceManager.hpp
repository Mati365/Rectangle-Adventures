/*
 * ResourceManager.hpp
 *
 *  Created on: 18-05-2013
 *      Author: mateusz
 */

#ifndef RESOURCEMANAGER_HPP_
#define RESOURCEMANAGER_HPP_
#include <stdio.h>
#include <iostream>

#include "../../Tools/Tools.hpp"
#include "../../Tools/Logger.hpp"
#include "../../Tools/Converter.hpp"

/**
 * TO JEST OSOBNA CZĘŚĆ NIE POWIĄZANA
 * Z SYSTEMEM PLIKÓW!!!
 * ------------------------
 * Menedżer zasobów gry. Tutaj
 * będą lądować plansze, 'modele'
 * i skrypty
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
		/**
		 * 1 argument - group ID
		 */
		deque<Resource*> resources;
		usint counter;

	public:
		ResourceManager() :
						counter(1) {
		}

		/**
		 * Zwracanie identyfikatoru
		 */
		usint addResource(Resource* _res) {
			resources.push_back(_res);
			_res->setResourceID(counter++);

			return counter;
		}

		/**
		 * Zwracanie identyfikatora
		 */
		usint getID(const char* _str) {
			Resource* res = getByLabel(_str);
			if (res) {
				return res->getResourceID();
			}
			return 0;
		}

		/**
		 * Zwracanie całego obiektu po label'u!
		 */
		Resource* getByLabel(const char* _str) {
			for (auto& res : resources) {
				if (strcmp(res->getLabel(), _str) == 0) {
					return res;
				}
			}
			return NULL;
		}

		/**
		 * Pobieranie elementów
		 */
		Resource* operator[](usint _id) {
			return getByID(_id);
		}

		Resource* getByID(usint _id) {
			for (auto& res : resources) {
				if (res->getResourceID() == _id) {
					return res;
				}
			}
			return NULL;
		}

		/**
		 * Kasowanie zasobu - wolne
		 */
		bool deleteResource(usint _id) {
			for (auto iter = resources.begin(); iter != resources.end();
					++iter) {
				if ((*iter)->getResourceID() == _id) {
					delete (*iter);
					resources.erase(iter);
					return true;
				}
			}
			return false;
		}

		/** Kasowanie niedobitków */
		~ResourceManager() {
			usint count = 0;
			for (auto& obj : resources) {
				if (!obj) {
					continue;
				}
				delete obj;
				count++;
			}
			logEvent(
					Logger::LOG_INFO,
					"Usunięto " + Convert::toString<usint>(count)
							+ " niedobitków!");
		}
};

#endif /* RESOURCEMANAGER_HPP_ */
