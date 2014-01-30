/*
 *  Copyright (C) 2009 Nicolas Vion <nico@picapo.net>
 *
 *   This file is part of Fluxy.
 *
 *   Fluxy is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Fluxy is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Fluxy; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <iostream>

#include "words_parser.hh"

bool WordsParser::waiting_for(std::string const str) {
	next();
	if (current != str) {
		std::cerr << "ERROR waiting for '" << str << "' given: '" << current << "'" << std::endl;
		return false;
	}
	return true;
}

char WordsParser::next_char() {
	return buffer->get();
}

std::string WordsParser::get_next(bool const allchars) {
	char c = buffer->get();
	if (buffer->eof()) 
		return "";

	//goto begining of word
	uint8_t c_class;
	while ((c_class = char_class[(int) c]) == CHAR_CLASS_SPACE) {
		c = buffer->get();
		if (buffer->eof()) 	
			return "";
	}

	//get chars
	if (allchars) {
		std::stringstream result;
		while (char_class[(int) c] != CHAR_CLASS_SPACE) {
			result << c;
			c = buffer->get();
			if (buffer->eof()) 
				return result.str();
		}
		buffer->unget();
		return result.str();
	}

	if (c_class == CHAR_CLASS_SEPARATOR) {
		return std::string(1, c);
	}

	std::stringstream result;
	while (char_class[(int) c] == c_class) {
		result << c;
		c = buffer->get();
		if (buffer->eof()) 
			return result.str();
	}
	buffer->unget();
	return result.str();
}

std::string WordsParser::next() {
	return current = get_next(false);
}

std::string WordsParser::until_space() {
	return current = get_next(true);
}

std::string WordsParser::until(char const last) {
	std::stringstream result;
	char c = buffer->get();
	while (c != last and !buffer->eof()) {
		result << c;
		c = buffer->get();
	}
	current = result.str();
	return current;
}

std::string WordsParser::until_end() {
	std::stringstream result;
	char c = buffer->get();
	while (!buffer->eof()) {
		result << c;
		c = buffer->get();
	}
	current = result.str();
	return current;
}

void WordsParser::set_char_class(std::string const chars, uint8_t const value) {
	int size = chars.size();
	for (int i = 0; i < size; i++)
		char_class[(int) chars[i]] = value;
}

WordsParser::WordsParser(std::istream *in) {
	buffer = in;
	current = "<undefined>";

	for (int i = 0; i < 255; i++) 
		char_class[i] = CHAR_CLASS_UNDEF;

	set_char_class(" \n\r\t", CHAR_CLASS_SPACE);
	set_char_class(",()[]{};!*#\"/", CHAR_CLASS_SEPARATOR);
	set_char_class("=<>|", CHAR_CLASS_1);
	set_char_class("&", CHAR_CLASS_2);
}
