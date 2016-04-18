//
// Created by epicfail on 29.03.16.
//

#ifndef TREE_ERROR_H
#define TREE_ERROR_H

#include <stdio.h>

//! @brief use in other files there are we throw something
#define THROW( type_error )     {\
                                    Error oh_now_I_have_an_error (type_error, __FILE__, __LINE__, __PRETTY_FUNCTION__);\
                                    throw oh_now_I_have_an_error;\
                                }
#define WPRINT( type_error )    Error::print_warning (type_error, __FILE__, __LINE__, __PRETTY_FUNCTION__);
#define EPRINT( type_error )    {\
                                    Error oh_now_I_have_an_error (type_error, __FILE__, __LINE__, __PRETTY_FUNCTION__);\
                                    oh_now_I_have_an_error.print_error ();\
                                }

#define NAME_LEN       40

//{-------------------------------------------------------------------
//! @brief enum with all errors
//!
//! @warning suffix W if warning
//}-------------------------------------------------------------------
enum error_type
{
    NULL_POINTER,       //! pointer is null
    BAD_NODE,           //! something in this node is bad
    CANNOT_OPEN_FILE,   //! name speaks for itself. Doesn't it?
    BAD_EXPRESSION,     //! something is wrong in input expression
    W_NOT_ALL_READ,     //! this warning is throwing if not all file was read
    CANNOT_ALLOC_MEMORY,//! I think you understand me:)
    NULL_EXPRESSION,    //! expression wasn't loaded in the object
    TOO_BIG_NUMBER,     //! int overflow
    BAD_IF_OR_WHILE     //! in code
};

//! @brief class to work with errors
class Error
{
public:

    //{-------------------------------------------------------------------
    //! @brief use it, when you are throwing an error
    //!
    //! @param err              some enum "error_type" meaning
    //! @param filename         name of the file where error occured
    //! @param line             line in the file where error occured
    //! @param pretty_function  name of the function where error occured
    //!
    //! @warning function write info into log_file__. If constructor can't open log_file__ all program aborting!
    //! @warning if !filename, line <= 0 or !pretty_function constructor write it in log_file__ and abort program
    //}-------------------------------------------------------------------
    Error (error_type err, const char * filename, int line, const char * pretty_function);

    ~Error ();

    //{-------------------------------------------------------------------
    //! @brief used when we catch error
    //!
    //! @param object of class Error
    //!
    //! @warning if !filename, line <= 0 or !pretty_function constructor write it in log_file__ and abort program
    //}-------------------------------------------------------------------
    Error (Error & that);

    //{-------------------------------------------------------------------
    //! @brief print last error into console
    //}-------------------------------------------------------------------
    void print_error ();

    //{-------------------------------------------------------------------
    //! @brief global function which print warnings in file
    //!
    //! @param err              error from enum error_type which can be regarded as a warning
    //! @param filename         name of the file where error occured
    //! @param line             line in the file where error occured
    //! @param pretty_function  name of the function where error occured
    //!
    //! @warning function write info into log_file__. If it can't open log_file__ all program aborting!
    //! @warning if !filename, line <= 0 or !pretty_function function write it in log_file__ and abort program
    //}-------------------------------------------------------------------
    static void print_warning (error_type err, const char * filename, int line, const char * pretty_function);

private:

    //{-------------------------------------------------------------------
    //! @brief file where the error occured
    //!
    //! @warning I'm so sorry, but I must use const size, because otherwise there is a cycle:(
    //}-------------------------------------------------------------------
    char filename_[NAME_LEN];

    //! @brief line where the error occured
    int line_;

    //{-------------------------------------------------------------------
    //! @brief function where the error occured
    //!
    //! @warning I'm so sorry, but I must use const size, because otherwise there is a cycle:(
    //}-------------------------------------------------------------------
    char function_[NAME_LEN];

    //! @brief number of error
    error_type error_;

    //{-------------------------------------------------------------------
    //! @brief class Error writing all in this file
    //!
    //! @warning log_file__ updates every session
    //}-------------------------------------------------------------------
    static FILE * log_file__;

    bool err_child_;
};

#endif //TREE_ERROR_H
