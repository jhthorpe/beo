/*****************************************
 * shared_file.hpp
 *
 * JHT, May 30, 2023, Dallas, TX
 *	- created
 *
 * Header file for the beo::Shared_File 
 *   class, which is file that can be
 *   written to by various MPI processes 
 *
 * Note that this file is ALWAYS created
 *   in binary mode
 *
 * Thread safety on a single node can be
 *   enforced via the lock(), unlock()
 *   member functions and access to the m
 *   member mutexi. Note that this does 
 *   NOT help with MPI thread safety 
 *   -- for that you will need to use
 *   read_at and write_at functions
 *   to ensure you are reading/writing
 *   where you think you want to be.
 *
*****************************************/
#ifndef _BEO_SHARED_FILE_
#define _BEO_SHARED_FILE_

#if defined _BEO_MPI_
#include <mpi.h>
#endif

#include <stdio.h>
#include <string>
#include <mutex>

#include "def.hpp"
#include "comm.hpp"
#include "request.hpp"

namespace beo
{

class Shared_File
{
    public:

        #if defined _BEO_MPI_

        using file_t = MPI_File;

        #else 

        using file_t = FILE*;  

        #endif

        using name_t = std::string;

        using key_t = name_t;

        using mutex_t = std::recursive_mutex;

        mutex_t m;

    protected:
 
        file_t  file_;

        name_t  name_;

        bool    is_open_{false};

    public:

        Shared_File(const std::string& name) {name_ = name;}

       ~Shared_File() {}

        Shared_File(const Shared_File& other);

        Shared_File(Shared_File&& other);

        const name_t& name() const {return name_;}

        const file_t& file() const {return file_;}

        int open(Comm& comm, const std::string& mode);

        int close();

        bool is_open() const {return is_open_;}

        BEO_OFF_T get_pos(); 

        int seek(const BEO_OFF_T offset);

        void lock() {m.lock();}

        void unlock() {m.unlock();}

        int write(const void* buf, const size_t bytes);

        int read(void* buf, const size_t bytes);

        int write_at(const BEO_OFF_T off, const void* buf, const size_t bytes);

        int read_at(const BEO_OFF_T off, void* buf, const size_t bytes);

        int write_at_all(const BEO_OFF_T off, const void* buf, const size_t bytes);

        int read_at_all(const BEO_OFF_T off, void* buf, const size_t bytes);

        Request async_write_at(const BEO_OFF_T off, const void* buf, const size_t bytes);

        Request async_read_at(const BEO_OFF_T off, void* buf, const size_t bytes);

        Request async_write_at_all(const BEO_OFF_T off, const void* buf, const size_t bytes);

        Request async_read_at_all(const BEO_OFF_T off, void* buf, const size_t bytes);

};

/*****************************************
 * Constructors
*****************************************/
Shared_File::Shared_File(const Shared_File& cother)
{
    auto& other = const_cast<Shared_File&>(cother);

    std::lock_guard<mutex_t> g1(m);
    std::lock_guard<mutex_t> g2(other.m);

    file_ = other.file_;
    name_ = other.name_;
}

Shared_File::Shared_File(Shared_File&& other)
{
    std::lock_guard<mutex_t> g1(m);
    std::lock_guard<mutex_t> g2(other.m);

    file_ = std::move(other.file_);
    name_ = std::move(other.name_);
}

/*****************************************
 * async_read_at
 *
 * Returns a beo::Request that, when 
 * completed, will have called read_at
*****************************************/
Request Shared_File::async_read_at(const BEO_OFF_T off,
                                   void* buf, 
                                   const size_t bytes)
{
    #if defined _BEO_MPI_
    
    MPI_Request fake;

    int stat = MPI_File_iread_at(file_,
                                 off, 
                                 buf,
                                 bytes,
                                 MPI_CHAR, 
                                 &fake);

    Request request = std::move(fake);
  
    return request;

    #else

    Request request = std::async(std::launch::async, [=]()
    {
        return read_at(off, buf, bytes);
    });
 
    return request;

    #endif
}

/*****************************************
 * async_write_at
 *
 * Returns a beo::Request that, when 
 * completed, will have called write_at
*****************************************/
Request Shared_File::async_write_at(const BEO_OFF_T off,
                                    const void* buf, 
                                    const size_t bytes)
{
    #if defined _BEO_MPI_
    
    MPI_Request fake;

    int stat = MPI_File_iwrite_at(file_,
                                  off, 
                                  buf,
                                  bytes,
                                  MPI_CHAR, 
                                  &fake);

    Request request = std::move(fake);
  
    return request;

    #else

    Request request = std::async(std::launch::async, [=]()
    {
        return write_at(off, buf, bytes);
    });
 
    return request;

    #endif
}

/*****************************************
 * async_read_at_all
 *
 * Returns a beo::Request that, when 
 * completed, will have called read_at_all. 
 * 
 * In the case of MPI, this calls the 
 * collective iread_at_all function, which 
 * is generally more efficient than idividual 
 * read/write operations.
 *
*****************************************/
Request Shared_File::async_read_at_all(const BEO_OFF_T off,
                                       void* buf, 
                                       const size_t bytes)
{
    #if defined _BEO_MPI_
    
    MPI_Request fake;

    int stat = MPI_File_iread_at_all(file_,
                                     off, 
                                     buf,
                                     bytes,
                                     MPI_CHAR, 
                                     &fake);

    Request request = std::move(fake);
  
    return request;

    #else

    Request request = std::async(std::launch::async, [=]()
    {
        return read_at_all(off, buf, bytes);
    });
 
    return request;

    #endif
}

/*****************************************
 * async_write_at_all
 *
 * Returns a beo::Request that, when 
 * completed, will have called write_at_all
 *
 * In the case of MPI, this calls the 
 * collective iwrite_at_all function, which 
 * is generally more efficient than idividual 
 * read/write operations.
 *
*****************************************/
Request Shared_File::async_write_at_all(const BEO_OFF_T off,
                                        const void* buf, 
                                        const size_t bytes)
{
    #if defined _BEO_MPI_
    
    MPI_Request fake;

    int stat = MPI_File_iwrite_at_all(file_,
                                      off, 
                                      buf,
                                      bytes,
                                      MPI_CHAR, 
                                      &fake);

    Request request = std::move(fake);
  
    return request;

    #else

    Request request = std::async(std::launch::async, [=]()
    {
        return write_at_all(off, buf, bytes);
    });
 
    return request;

    #endif
}

/*****************************************
 * read_at 
 *
 * read from the file from a buffer, 
 *   at a location some off bytes from 
 *   SEEK_SET 
 *
 * The seek and the read as locked 
 * together to ensure thread-safety
*****************************************/
int Shared_File::read_at(const BEO_OFF_T off, 
                         void* buf,
                         const size_t bytes)
{
    std::lock_guard<mutex_t> g(m);

    #if defined _BEO_MPI_

    if (bytes == 0) return BEO_SUCCESS;

    return (MPI_SUCCESS == MPI_File_read_at(file_,
                                            off,
                                            buf, 
                                            bytes, 
                                            MPI_CHAR,
                                            MPI_STATUS_IGNORE))
           ? BEO_SUCCESS : BEO_FAIL;

    #else

    if (bytes == 0) return BEO_SUCCESS;

    seek(off);

    return (0 != fread(buf,
                       bytes,
                       1, 
                       file_))
           ? BEO_SUCCESS : BEO_FAIL;

    #endif
}

/*****************************************
 * write_at 
 *
 * read from the file from a buffer, 
 *   at a location some off bytes from 
 *   SEEK_SET 
 *
 * The seek and the read as locked 
 * together to ensure thread-safety 
*****************************************/
int Shared_File::write_at(const BEO_OFF_T off, 
                          const void* buf,
                          const size_t bytes)
{

    std::lock_guard<mutex_t> g(m);

    #if defined _BEO_MPI_

    if (bytes == 0) return BEO_SUCCESS;

    return (MPI_SUCCESS == MPI_File_write_at(file_,
                                             off,
                                             buf, 
                                             bytes, 
                                             MPI_CHAR,
                                             MPI_STATUS_IGNORE))
           ? BEO_SUCCESS : BEO_FAIL;

    #else

    if (bytes == 0) return BEO_SUCCESS;

    seek(off);

    return (0 != fwrite(buf,
                        bytes,
                        1, 
                        file_))
           ? BEO_SUCCESS : BEO_FAIL;

    #endif
}


/*****************************************
 * read_at_all 
 *
 * read from the file from a buffer, 
 *   at a location some off bytes from 
 *   SEEK_SET 
 *
 * The seek and the read as locked 
 * together to ensure thread-safety.
 *
 * In the case of MPI, this calls the 
 * collective iread_at_all function, which 
 * is generally more efficient than idividual 
 * read/write operations.
 *
*****************************************/
int Shared_File::read_at_all(const BEO_OFF_T off, 
                             void* buf,
                             const size_t bytes)
{
    std::lock_guard<mutex_t> g(m);

    #if defined _BEO_MPI_

    if (bytes == 0) return BEO_SUCCESS;

    return (MPI_SUCCESS == 
            MPI_File_read_at_all(file_,
                                 off,
                                 buf, 
                                 bytes, 
                                 MPI_CHAR,
                                 MPI_STATUS_IGNORE))
           ? BEO_SUCCESS : BEO_FAIL;

    #else

    return read_at(off, buf, bytes);

    #endif
}

/*****************************************
 * write_at_all 
 *
 * read from the file from a buffer, 
 *   at a location some off bytes from 
 *   SEEK_SET 
 *
 * The seek and the read as locked 
 * together to ensure thread-safety 
 *
 * In the case of MPI, this calls the 
 * collective iread_at_all function, which 
 * is generally more efficient than idividual 
 * read/write operations.
 *
*****************************************/
int Shared_File::write_at_all(const BEO_OFF_T off, 
                              const void* buf,
                              const size_t bytes)
{

    std::lock_guard<mutex_t> g(m);

    #if defined _BEO_MPI_

    if (bytes == 0) return BEO_SUCCESS;

    return (MPI_SUCCESS == MPI_File_write_at_all(file_,
                                                 off,
                                                 buf, 
                                                 bytes, 
                                                 MPI_CHAR,
                                                 MPI_STATUS_IGNORE))
           ? BEO_SUCCESS : BEO_FAIL;

    #else
  
    return write_at(off, buf, bytes);

    #endif
}


/*****************************************
 * read 
 *
 * read from the file from a buffer. 
 * Note that this does nothing to ensure
 * threadsafety of any kind.
*****************************************/
int Shared_File::read(void* buf,
                      size_t bytes)
{

    #if defined _BEO_MPI_

    if (bytes == 0) return BEO_SUCCESS;

    return (MPI_SUCCESS == MPI_File_read(file_,
                                         buf, 
                                         bytes, 
                                         MPI_CHAR,
                                         MPI_STATUS_IGNORE))
           ? BEO_SUCCESS : BEO_FAIL;

    #else

    if (bytes == 0) return BEO_SUCCESS;

    return (0 != fread(buf,
                       bytes,
                       1, 
                       file_))
           ? BEO_SUCCESS : BEO_FAIL;

    #endif
}

/*****************************************
 * write
 *
 * write to the file from a buffer. 
 * Note that this does nothing to ensure
 * threadsafety of any kind.
*****************************************/
int Shared_File::write(const void* buf,
                       size_t bytes)
{

    #if defined _BEO_MPI_

    if (bytes == 0) return BEO_SUCCESS;

    return (MPI_SUCCESS == MPI_File_write(file_,
                                          buf, 
                                          bytes, 
                                          MPI_CHAR,
                                          MPI_STATUS_IGNORE))
           ? BEO_SUCCESS : BEO_FAIL;

    #else

    if (bytes == 0) return BEO_SUCCESS;

    return (0 != fwrite(buf,
                        bytes,
                        1, 
                        file_))
           ? BEO_SUCCESS : BEO_FAIL;

    #endif
}

/*****************************************
 * seek
 *
 * Seeks to an offset in bytes from 
 * SEEK_SET 
*****************************************/
int Shared_File::seek(const BEO_OFF_T off)
{
    #if defined _BEO_MPI_
 
    return (MPI_SUCCESS == MPI_File_seek(file_, off, MPI_SEEK_SET))
           ? BEO_SUCCESS : BEO_FAIL;
    
    #else

    return (0 == fseek(file_, off, SEEK_SET))
           ? BEO_SUCCESS : BEO_FAIL;

    #endif
}


/*****************************************
 * get_pos
 *
 * returns current offset of file
 * 
 * returns -1 if the position is bad
*****************************************/
BEO_OFF_T Shared_File::get_pos() 
{
    #if defined _BEO_MPI_
  
    MPI_Offset pos;
 
    int stat = MPI_File_get_position(file_, &pos);

    return (MPI_SUCCESS == stat) ? (BEO_OFF_T) pos : -1; 

    #else

    BEO_OFF_T pos = BEO_FTELL(file_);
   
    return (pos >= 0) ? pos : -1;

    #endif
}

/*****************************************
 * open
 *
 * dne == "Does not exist"
 * Modes correspond to usual fopen modes
 *
 * modes:
 *   r 	: read, error if dne
 *   r+	: read/write, error if dne
 *   w	: write, overwrites if exist
 *   w+	: read/write, overwrites if exist
*****************************************/
int Shared_File::open(Comm& comm, const std::string& mode)
{
    if (is_open())
    {
        printf("beo::error Cannot open already opened file\n");
        return BEO_FAIL;
    }

    #if defined _BEO_MPI_
     
    int mpi_mode;
    if      ("w+" == mode) mpi_mode = MPI_MODE_CREATE | MPI_MODE_RDWR; 
    else if ("r+" == mode) mpi_mode = MPI_MODE_RDWR;
    else if ("r"  == mode) mpi_mode = MPI_MODE_RDONLY; 
    else if ("w"  == mode) mpi_mode = MPI_MODE_CREATE | MPI_MODE_WRONLY;
    else
    {
        printf("beo::error Bad mode %s in beo::shared_file::open\n", mode.c_str());
        return BEO_FAIL;
    }

    is_open_ = (nullptr != file_) ? true : false; 

    return (MPI_SUCCESS ==
            MPI_File_open(comm.comm(), 
                          name_.data(),
                          mpi_mode,
                          MPI_INFO_NULL,
                          &file_))
        ? BEO_SUCCESS : BEO_FAIL;

    #else

    file_ = fopen(name_.data(), 
                  mode.data());
   
    is_open_ = (nullptr != file_) ? true : false; 

    return (nullptr != file_) ? BEO_SUCCESS : BEO_FAIL; 

    #endif
}

/*****************************************
 * close
 *
*****************************************/
int Shared_File::close()
{
    if (!is_open()) return BEO_SUCCESS;
   
    #if defined _BEO_MPI_

    int stat = MPI_SUCCESS;

    if (MPI_FILE_NULL != file_) 
    {
        stat = MPI_File_close(&file_);
        file_ = MPI_FILE_NULL;
    }

    return (MPI_SUCCESS == stat) ? BEO_SUCCESS : BEO_FAIL;

    #else
   
    int stat = 0;

    if (nullptr != file_) 
    {
        stat = fclose(file_);
        file_ = nullptr;
    }

    return (nullptr != file_) ? BEO_SUCCESS : BEO_FAIL; 

    #endif
}

} //end namespace beo

#endif
