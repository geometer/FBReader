#ifndef __ZLTREEDIALOG_H__
#define __ZLTREEDIALOG_H__

#include <ZLTreeListener.h>

// This object should be pure successor of ZLTreeListener
// or be merged with it.
class ZLTreeDialog : public ZLTreeListener {

protected:
	ZLTreeDialog();

public:
	virtual ~ZLTreeDialog();

	// TODO: change return type to int (or std::string?)
	virtual bool run() = 0;

protected:
};

#endif /* __ZLTREEDIALOG_H__ */
