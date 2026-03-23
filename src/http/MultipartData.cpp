/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultipartData.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 13:58:44 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/23 16:00:40 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "http/MultipartData.hpp"
#include "utils/Utils.hpp"

// ############################ MultipartData Class ############################

//-------------------------- Destructor / Constructor --------------------------

MultipartData::MultipartData(void)
{
}

MultipartData::~MultipartData(void)
{
    std::cout << "MultipartData destructor called" << std::endl;
}

MultipartData::MultipartData(const MultipartData& copy) : fields_(copy.fields_), files_(copy.files_)
{
}

MultipartData& MultipartData::operator=(const MultipartData& copy)
{
    if (this != &copy)
    {
        fields_ = copy.fields_;
        files_ = copy.files_;
    }
    return (*this);
}

// --------------------------- Public Member Methods ---------------------------

void                                        MultipartData::addField( const std::string& name, const std::string& value )
{
    fields_[Utils::toLower(name)].push_back(value);
}

bool                                        MultipartData::hasField( const std::string& name ) const
{
    return ( fields_.find(Utils::toLower(name)) != fields_.end() );
}

const std::vector<std::string>&                          MultipartData::getField( const std::string& name ) const
{
    static const std::vector<std::string> empty;

    std::map<std::string, std::vector<std::string> >::const_iterator it = fields_.find(Utils::toLower(name));
    if (it != fields_.end())
        return (it->second);
    return (empty);
}

const std::map<std::string, std::vector<std::string> >&   MultipartData::getAllFields() const
{
    return (fields_);
}

size_t          MultipartData::getFieldCount( const std::string& name ) const
{
    size_t  count = 0;

    std::map<std::string, std::vector<std::string> >::const_iterator it = fields_.find(Utils::toLower(name));
    if (it != fields_.end())
        return (it->second.size());
    return (count);
}


void                                        MultipartData::addFile( const std::string& name, const UploadedFile& filename )
{
    files_[Utils::toLower(name)].push_back(filename);
}

bool                                        MultipartData::hasFile( const std::string& name ) const
{
    return ( files_.find(Utils::toLower(name)) != files_.end() );

}

const std::vector<UploadedFile>&                         MultipartData::getFiles( const std::string& name ) const
{
    static const std::vector<UploadedFile> empty;
    std::map<std::string, std::vector<UploadedFile> >::const_iterator it = files_.find(Utils::toLower(name));
    if (it != files_.end())
        return (it->second);
    return (empty);
}

const std::map<std::string, std::vector<UploadedFile> >&  MultipartData::getAllFiles() const
{
    return (files_);
}

size_t          MultipartData::getFileCount( const std::string& name ) const
{
    size_t  count = 0;

    std::map<std::string, std::vector<UploadedFile> >::const_iterator it = files_.find(Utils::toLower(name));
    if (it != files_.end())
        return (it->second.size());
    return (count);
}
