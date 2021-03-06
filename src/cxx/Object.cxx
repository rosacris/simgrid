/*
 * Object.cxx
 *
 * Copyright 2006,2007 Martin Quinson, Malek Cherier           
 * All right reserved. 
 *
 * This program is free software; you can redistribute 
 * it and/or modify it under the terms of the license 
 *(GNU LGPL) which comes with this package. 
 *
 */
 
 /* SimGrid::Msg RTTI implementation.
  */  

#include <Object.hpp>
#include <string.h>

#include <xbt/dynar.h>




DeclaringClasses* DeclaringClass::declaringClasses = NULL;

namespace SimGrid
{
	namespace Msg
	{

		// Generate static object constructor for class registration
		void DeclareClass(Class* c)
		{
			MSG_DELCARING_CLASSES.lock();
			MSG_DELCARING_CLASSES.addHead(c);
			MSG_DELCARING_CLASSES.unlock();
		}
	} // namespace Msg
} // namespace SimGrid

//////////////////////////////////////////////////////////////////////////////
// Impl�mentation des fonctions membre de la classe Class

// true if the class is derived from base classe referenced 
// by pBaseClass
bool Class::isDerivedFrom(const Class* baseClass) const
{
	const Class* cur = this;

	while(cur)
	{
		if(cur == baseClass)
			return true;

		cur = cur->baseClass;
	}

	return false;
}

// Dynamic name lookup and creation
Class* Class::fromName(const char* name)
throw (ClassNotFoundException)
{
	Class* cur;

	MSG_DELCARING_CLASSES.lock();
	
	for(cur = MSG_DELCARING_CLASSES.getHead(); cur; cur = cur->next)
	{
		if(!strcmp(name,cur->name))
		{
			MSG_DELCARING_CLASSES.unlock();
			return cur;
		}
	}

	MSG_DELCARING_CLASSES.unlock();
	throw ClassNotFoundException(name);
}


Object* Class::createObject(const char* name)
{
	Class* c = fromName(name);
	return c->createObject();
}


//////////////////////////////////////////////////////////////////////////////
// Object members implementation

// Special runtime-class structure for Object (no base class)
const struct Class Object::classObject =
{ 
	"Object", 		// name
	sizeof(Object),	// typeSize
	NULL,			// baseClass
	NULL, 			// next
	NULL			// declaringClass
};


//////////////////////////////////////////////////////////////////////////////
// DeclaringClasses members implementation
//

DeclaringClasses::DeclaringClasses()
{
        head = NULL;
        count = 0;
}


// Ajoute une nouvelle classe en t�te de liste
void DeclaringClasses::addHead(Class* c)
{
	if(NULL != head)
		c->next = head;

	head = c;
    count++;
}

// Retourne la t�te de liste et la retire de la liste
Class* DeclaringClasses::removeHead(void)
{
	Class* c = NULL;

	if(NULL != head)
	{
		c = head;
		head = head->next;
		count--;
	}

	return c;
}

// Retire la classe pClasse de la liste, mais ne la d�truit pas 
bool DeclaringClasses::remove(Class* c)
{
	if(NULL == head)
		return false;

	bool success = false;

	if(head == c)
	{
		head = c->next;
        count--;
		success = true;
	}
	else
	{
		Class* cur = head;

		while((NULL != cur) && (cur->next!= c))
			cur = cur->next;

		if(NULL != cur)
		{
			cur->next = c->next;
            count--;
			success = true;
		}
	}
	
	return success;
}

bool Object::isInstanceOf(const char* className)
{
	Class* c = Class::fromName(className);

	return this->getClass()->isDerivedFrom(c);
}
		

