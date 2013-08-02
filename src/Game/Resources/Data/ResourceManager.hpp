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
 * TO JEST OSOBNA CZĘŚĆ NIE POWIĄZANA
 * Z SYSTEMEM PLIKÓW!!!
 * ------------------------
 * Menedżer zasobów gry. Tutaj
 * będą lądować plansze, 'modele'
 * i skrypty
 */
template<class ID>
class Resource {
	protected:
		char* label;
		ID resource_id;

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
		void setResourceID(ID _id) {
			resource_id = _id;
		}
		ID getResourceID() const {
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

template<class ID>
class ResourceManager {
	private:
		/**
		 * 1 argument - group ID
		 */
		deque<AllocKiller<Resource<ID> > > resources;

	public:
		/**
		 * Zwracanie identyfikatoru
		 */
		ID addResource(Resource<ID>* _res) {
			if (!_res) {
				logEvent(Logger::LOG_ERROR, "Błąd podczas dodawania zasobu!");
				return 0;
			}
			AllocKiller<Resource<ID> > alloc(_res);
			_res->setResourceID(resources.size());
			
			resources.push_back(alloc);
			return resources.size() - 1;
		}
		/**
		 * Zwracanie identyfikatora
		 */
		ID getID(const char* _str) {
			Resource<ID*> id = getByLabel(_str);
			if (id) {
				return id->getResourceID();
			}
			return 0;
		}
		/**
		 * Zwracanie całego obiektu po label'u!
		 */
		Resource<ID>* getByLabel(const char* _str) {
			if (strlen(_str) == 0) {
				return 0;
			}
			for (auto iter = resources.begin(); iter != resources.end();
					++iter) {
				if (strcmp(_str, (*iter)->getLabel()) == 0) {
					return (*iter);
				}
			}
			return NULL;
		}
		/**
		 * Pobieranie elementów
		 */
		Resource<ID>* operator[](ID _id) {
			return getByID(_id);
		}
		
		Resource<ID>* getByID(ID _id) {
			if (_id >= resources.size()) {
				return NULL;
			}
			return resources[reinterpret_cast<usint>(_id)];
		}
		/**
		 * Informacje nt. zasobów
		 */
		usint getSize() const {
			return resources.size();
		}
		/**
		 *
		 */
		void deleteResource(ID id) {
			resources.erase(resources.begin() + id);
		}
};

#endif /* RESOURCEMANAGER_HPP_ */
