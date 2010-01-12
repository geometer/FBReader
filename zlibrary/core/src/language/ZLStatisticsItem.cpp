/*
 * Copyright (C) 2009-2010 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "ZLStatisticsItem.h"

ZLMapBasedStatisticsItem::ZLMapBasedStatisticsItem(const std::map<ZLCharSequence, size_t>::const_iterator it, size_t index) : 	ZLStatisticsItem(index), 
																																myIterator(it) {
}

ZLCharSequence ZLMapBasedStatisticsItem::sequence() const {
	return myIterator->first;
}
  
size_t ZLMapBasedStatisticsItem::frequency() const {
	return myIterator->second;
}

void ZLMapBasedStatisticsItem::next() {
    ++myIndex;
	++myIterator;
}

ZLArrayBasedStatisticsItem::ZLArrayBasedStatisticsItem(char* sequencePtr, unsigned short* frequencyPtr, size_t index) : ZLStatisticsItem(index),
																														mySequencePtr(sequencePtr), 
																														myFrequencyPtr(frequencyPtr) {
}

ZLCharSequence ZLArrayBasedStatisticsItem::sequence() const {
	return ZLCharSequence(mySequencePtr, 3);
}

size_t ZLArrayBasedStatisticsItem::frequency() const {
	return (size_t) *myFrequencyPtr;
}

void ZLArrayBasedStatisticsItem::next() {
	++myIndex;
    mySequencePtr += 3;
    ++myFrequencyPtr;
}

