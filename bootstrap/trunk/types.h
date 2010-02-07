/*
 * This file is part of Google Wave Notifier.
 *
 * Google Wave Notifier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Google Wave Notifier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _INC_TYPES2
#define _INC_TYPES2

#pragma once

typedef vector<BYTE> TByteVector;
typedef TByteVector::iterator TByteVectorIter;
typedef TByteVector::const_iterator TByteVectorConstIter;
typedef vector<wstring> TStringVector;
typedef TStringVector::iterator TStringVectorIter;
typedef TStringVector::const_iterator TStringVectorConstIter;
typedef map<wstring, wstring> TStringStringMap;
typedef TStringStringMap::iterator TStringStringMapIter;
typedef TStringStringMap::const_iterator TStringStringMapConstIter;
typedef vector<RECT> TRectVector;
typedef TRectVector::iterator TRectVectorIter;
typedef TRectVector::const_iterator TRectVectorConstIter;
typedef vector<HWND> THwndVector;
typedef THwndVector::iterator THwndVectorIter;
typedef THwndVector::const_iterator THwndVectorConstIter;
typedef map<wstring, BOOL> TStringBoolMap;
typedef TStringBoolMap::iterator TStringBoolMapIter;
typedef TStringBoolMap::const_iterator TStringBoolMapConstIter;
typedef vector<UINT_PTR> TUintPtrVector;
typedef TUintPtrVector::iterator TUintPtrVectorIter;
typedef TUintPtrVector::const_iterator TUintPtrVectorConstIter;

#endif // _INC_TYPES2
