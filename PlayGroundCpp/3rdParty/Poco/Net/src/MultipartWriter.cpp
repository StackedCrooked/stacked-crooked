//
// MultipartWriter.cpp
//
// $Id: //poco/1.3/Net/src/MultipartWriter.cpp#3 $
//
// Library: Net
// Package: Messages
// Module:  MultipartWriter
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Net/MultipartWriter.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Random.h"
#include "Poco/NumberFormatter.h"


using Poco::Random;
using Poco::NumberFormatter;


namespace Poco {
namespace Net {


MultipartWriter::MultipartWriter(std::ostream& ostr):
	_ostr(ostr),
	_boundary(createBoundary()),
	_firstPart(true)
{
}


MultipartWriter::MultipartWriter(std::ostream& ostr, const std::string& boundary):
	_ostr(ostr),
	_boundary(boundary),
	_firstPart(true)
{
	if (_boundary.empty())
		_boundary = createBoundary();
}


MultipartWriter::~MultipartWriter()
{
}

	
void MultipartWriter::nextPart(const MessageHeader& header)
{
	if (_firstPart)
		_firstPart = false;
	else
		_ostr << "\r\n";
	_ostr << "--" << _boundary << "\r\n";
	header.write(_ostr);
	_ostr << "\r\n";
}

	
void MultipartWriter::close()
{
	_ostr << "\r\n--" << _boundary << "--\r\n";
}


const std::string& MultipartWriter::boundary() const
{
	return _boundary;
}


std::string MultipartWriter::createBoundary()
{
	std::string boundary("MIME_boundary_");
	Random rnd;
	NumberFormatter::appendHex(boundary, rnd.next(), 8);
	NumberFormatter::appendHex(boundary, rnd.next(), 8);
	return boundary;
}


} } // namespace Poco::Net
