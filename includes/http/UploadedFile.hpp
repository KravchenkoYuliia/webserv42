/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadedFile.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 15:58:22 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/23 15:57:32 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADEDFILE_HPP
#define UPLOADEDFILE_HPP

#include <string>

class UploadedFile {
public:
    // ---------- Constructors / Destructor ----------

    // UploadedFile(void);
    UploadedFile(const std::string& filename, const std::string& content_type, const std::string& data);
    UploadedFile(const UploadedFile& copy);
    UploadedFile& operator=(const UploadedFile& copy);

    ~UploadedFile(void);

    // ---------- Overloading Operators Methods -------


    // ---------- Getter and Setter Methods ------------

    const std::string&  getFilename() const;
    const std::string&  getContentType() const;
    const std::string&  getData() const;

    void                setFilename(const std::string& filename);
    void                setContentType(const std::string& content_type);
    void                setData(const std::string& data);

    // ---------- Member Methods -----------------------

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    std::string filename_;
    std::string content_type_;
    std::string data_;
};

#endif // UPLOADEDFILE_HPP
