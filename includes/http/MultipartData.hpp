/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultipartData.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 11:25:42 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/23 11:23:02 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPART_DATA_HPP
#define MULTIPART_DATA_HPP

#include <string>
#include <vector>
#include <map>
#include "http/UploadedFile.hpp"

class MultipartData {
public:
    // ---------- Constructors / Destructor ----------

    MultipartData(void);
    ~MultipartData(void);
    MultipartData(const MultipartData& copy);
    MultipartData& operator=(const MultipartData& copy);

    // ---------- Member Methods -----------------------
    void                                                        addField( const std::string& name, const std::string& value );
    bool                                                        hasField( const std::string& name ) const;
    const std::vector<std::string>&                             getField( const std::string& name ) const;
    const std::map<std::string, std::vector<std::string> >&     getAllFields() const;
    size_t                                                      getFieldCount( const std::string& name ) const;

    void                                                        addFile( const std::string& name, const UploadedFile& filename );
    bool                                                        hasFile( const std::string& name ) const;
    const std::vector<UploadedFile>&                            getFiles( const std::string& name ) const;
    const std::map<std::string, std::vector<UploadedFile> >&    getAllFiles() const;
    size_t                                                      getFileCount( const std::string& name ) const;


protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    std::map<std::string, std::vector<std::string> >  fields_;
    std::map<std::string, std::vector<UploadedFile> > files_;
};

#endif // MULTIPART_DATA_HPP
