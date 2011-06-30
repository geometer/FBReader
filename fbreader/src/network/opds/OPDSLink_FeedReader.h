/*
 * qqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
 */

#ifndef __OPDSLink__FeedReader_H__
#define __OPDSLink__FeedReader_H__

#include <map>
#include <string>

#include "OPDSFeedReader.h"
#include "OPDSLink.h"

class OPDSLink::FeedReader : public OPDSFeedReader {

public:
	FeedReader(
		shared_ptr<NetworkLink>& link,
		const std::string &baseURL
	);

public:
	void processFeedEntry(shared_ptr<OPDSEntry> entry);
	void processFeedStart();
	void processFeedMetadata(shared_ptr<OPDSFeedMetadata> feed);
	void processFeedEnd();


	const std::string myBaseURL;
	shared_ptr<NetworkLink>& myLink;
};













#endif /* __OPDSLink__FeedReader_H__ */
