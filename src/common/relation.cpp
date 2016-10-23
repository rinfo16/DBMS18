#include "common/relation.h"
#include <sstream>

boost::property_tree::ptree PageIDToPropertyTree(PageID pageid)
{
  boost::property_tree::ptree tree;
  tree.put(STR_FILE_NO, pageid.fileno_);
  tree.put(STR_PAGE_NO, pageid.blockno_);

  return tree;
}

PageID PropertyTreeToPageID(boost::property_tree::ptree tree)
{
  PageID pageid;
  std::stringstream ssm;
  ssm << tree.get<fileno_t>(STR_FILE_NO);
  ssm >> pageid.fileno_;
  ssm.clear();
  ssm << tree.get<pageno_t>(STR_PAGE_NO);
  ssm >> pageid.blockno_;

  return pageid;
}
