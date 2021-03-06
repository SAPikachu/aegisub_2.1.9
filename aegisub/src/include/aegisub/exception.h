// Copyright (c) 2009, Niels Martin Hansen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#include <wx/string.h>
#pragma once

namespace Aegisub {

	// Base class for exceptions
	// No public creators, all exceptions throws must be specific
	class Exception {
		wxString message;
		Exception *inner;

	protected:
		Exception(const wxString &msg, const Exception *inr = 0)
			: message(msg)
			, inner(0)
		{
			if (inr)
				inner = inr->Copy();
		}
		Exception(); // not implemented, not wanted
		virtual ~Exception() { if (inner) delete inner; }
	public:
		// Error message for outer exception
		virtual wxString GetMessage() const { return message; }
		// Error message for outer exception, and chained message for inner exception
		wxString GetChainedMessage() const { if (inner) return inner->GetChainedMessage() + _T("\r\n") + GetMessage(); else return GetMessage(); }
		// Name of exception class, should only be implemented by specific classes
		virtual const wxChar * GetName() const = 0;

		operator const wxChar * () { return GetMessage().c_str(); }
		operator wxString () { return GetMessage(); }

		/// @brief Create a copy of the exception allocated on the heap
		/// @return A heap-allocated exception object
		///
		/// All deriving classes must implement this explicitly to avoid losing
		/// information in the duplication.
		virtual Exception *Copy() const = 0;
	};


	// Macro to quickly add location information to an error message
#define AG_WHERE _T(" (at ") _T(__FILE__) _T(":") _T(#__LINE__) _T(")")


	// Macros to define basic exception classes that do nothing fancy
	// These should always be used inside the Aegisub namespace
#define DEFINE_SIMPLE_EXCEPTION_NOINNER(classname,baseclass,displayname)             \
	class classname : public baseclass {                                             \
	public:                                                                          \
		classname(const wxString &msg) : baseclass(msg) { }                          \
		const wxChar * GetName() const { return _T(displayname); }                   \
		Exception * Copy() const { return new classname(*this); }                    \
	};
#define DEFINE_SIMPLE_EXCEPTION(classname,baseclass,displayname)                     \
	class classname : public baseclass {                                             \
	public:                                                                          \
		classname(const wxString &msg, Exception *inner) : baseclass(msg, inner) { } \
		const wxChar * GetName() const { return _T(displayname); }                   \
		Exception * Copy() const { return new classname(*this); }                    \
	};
#define DEFINE_BASE_EXCEPTION_NOINNER(classname,baseclass)                           \
	class classname : public baseclass {                                             \
	public:                                                                          \
		classname(const wxString &msg) : baseclass(msg) { }                          \
	};
#define DEFINE_BASE_EXCEPTION(classname,baseclass)                                   \
	class classname : public baseclass {                                             \
	public:                                                                          \
		classname(const wxString &msg, Exception *inner) : baseclass(msg, inner) { } \
	};


	// Exception for "user cancel" events
	// I.e. when we want to abort an operation because the user requested that we do so
	// Not actually an error and should not be handled as such
	DEFINE_SIMPLE_EXCEPTION_NOINNER(UserCancelException,Exception,"nonerror/user_cancel")


	// Errors that should never happen and point to some invalid assumption in the code
	DEFINE_SIMPLE_EXCEPTION(InternalError, Exception, "internal_error")


	// Some error related to the filesystem
	// These should always be original causes and as such do not support inner exceptions
	DEFINE_BASE_EXCEPTION_NOINNER(FileSystemError,Exception)

	// A file can't be accessed for some reason
	DEFINE_SIMPLE_EXCEPTION_NOINNER(FileNotAccessibleError,FileSystemError,"filesystem/not_accessible")

	// A file isn't accessible because it doesn't exist
	class FileNotFoundError : public FileNotAccessibleError {
	public:
		FileNotFoundError(const wxString &filename) : FileNotAccessibleError(wxString(_T("File not found: ")) + filename) { }
		const wxChar * GetName() const { return _T("filesystem/not_accessible/not_found"); }

		// Not documented, see  Aegisub::Exception class
		Exception * Copy() const { return new FileNotFoundError(*this); }                    \
	};


	// A problem with some input data
	DEFINE_BASE_EXCEPTION(InvalidInputException,Exception)


	// There is no "generic exception" class, everything must be a specific one
	// Define new classes if none fit the error you're reporting

};
