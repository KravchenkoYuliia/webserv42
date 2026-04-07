/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadedFile.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 11:11:01 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 18:38:34 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "http/HttpConstants.hpp"
#include "http/UploadedFile.hpp"

// ############################ UploadedFile Class ############################

// ------------------------- Destructor / Constructor -------------------------

// UploadedFile::UploadedFile()
//     :   filename_("default_filename"),
//         content_type_(Http::ContentType::TEXT_PLAIN),
//         data_("")
// {}

UploadedFile::UploadedFile(
    const std::string& filename,
    const std::string& content_type,
    const std::string& data
)   :   filename_(filename),
        content_type_(content_type),
        data_(data)
{
}

UploadedFile::UploadedFile(const UploadedFile& copy)
    :   filename_(copy.filename_),
        content_type_(copy.content_type_),
        data_(copy.data_)
{

}

UploadedFile& UploadedFile::operator=(const UploadedFile& copy)
{
    if (this != &copy)
    {
        filename_ = copy.filename_;
        content_type_ = copy.content_type_;
        data_ = copy.data_;
    }
    return (*this);
}

UploadedFile::~UploadedFile(void)
{
}

// --------------------------- Public Getter Methods ---------------------------
// --------------------------- Public Setter Methods ---------------------------

const std::string&  UploadedFile::getFilename() const
{
    return (filename_);
}

const std::string&  UploadedFile::getContentType() const
{
    return (content_type_);
}

const std::string&  UploadedFile::getData() const
{
    return (data_);
}

void                UploadedFile::setFilename(const std::string& filename)
{
    filename_ = filename;
}

void                UploadedFile::setContentType(const std::string& content_type)
{
    content_type_ = content_type;
}

void                UploadedFile::setData(const std::string& data)
{
    data_ = data;
}



//--------------------------- Private Member Methods ---------------------------

