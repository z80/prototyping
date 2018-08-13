
#ifndef __NAME_GENERATOR_H_
#define __NAME_GENERATOR_H_

#include <string>
#include "Ogre.h"

/**
 * \brief	A utility class to generate unique names.
**/
class NameGenerator
{
public:

    /**
     * \brief	Gets the next unique name.
    **/
    static std::string Next();

    /**
     * \brief	Gets the next unique name for a given prefix.
    **/
    static std::string Next( const std::string & prefix );

    /**
     * \brief	Counts the number of unique auto-generated names.
    **/
    static size_t Count();

    /**
     * \brief	Counts the number of names generated for a given prefix.
     * \param	prefix	The prefix of the generated names.
    **/
    static size_t Count(const std::string& prefix);
private:
    NameGenerator(void);
    ~NameGenerator(void);

    typedef std::map<std::string, Ogre::uint32> NameCountMap;
    static NameCountMap s_nameCount;
};




#endif


