/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include "ContextTree.h"

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

#include <pearl/CallingContext.h>
#include <pearl/Error.h>
#include <pearl/Region.h>
#include <pearl/String.h>
#include <pearl/SourceLocation.h>

#include "Functors.h"

using namespace std;
using namespace testing;
using namespace pearl;
using namespace pearl::detail;


//--- Identifier constants --------------------------------------------------

enum StringIds
{
    STR_EMPTY,
    STR_SOURCE,
    STR_MAIN,
    STR_FOO,
    STR_BAR,
    STR_OOB
};


enum RegionIds
{
    REG_MAIN,
    REG_FOO,
    REG_BAR,
    REG_OOB
};


enum SclIds
{
    SCL_MAIN,
    SCL_FOO,
    SCL_BAR,
    SCL_OOB
};


enum ContextIds
{
    CC_M,
    CC_M_F0,
    CC_M_F0_B,
    CC_M_F1,
    CC_M_F1_B,
    CC_OOB
};


//--- ContextTree tests -----------------------------------------------------

// Test fixture used for calling context definition tests
//
// Build up a calling context tree for the following source code
// (where each "@" source line corresponds to one sample):
//
//      1       void bar( void )
//      2       {
//      3           @fma
//      4       }
//      5
//      6       void foo( void )
//      7       {
//      8           @call bar()
//      9           @call bar()
//     10       }
//     11
//     12       int main( void )
//     13       {
//     14           @call foo()
//     15       }
//
class ContextTreeT
    : public Test
{
    public:
        virtual void
        SetUp()
        {
            mStrings.push_back(new String(STR_EMPTY, ""));
            mStrings.push_back(new String(STR_SOURCE, "test.c"));
            mStrings.push_back(new String(STR_MAIN, "main"));
            mStrings.push_back(new String(STR_FOO, "foo"));
            mStrings.push_back(new String(STR_BAR, "bar"));

            mRegions.push_back(new Region(REG_MAIN,
                                          *mStrings[STR_MAIN],
                                          *mStrings[STR_MAIN],
                                          *mStrings[STR_EMPTY],
                                          Region::ROLE_FUNCTION,
                                          Paradigm::SAMPLING,
                                          *mStrings[STR_SOURCE],
                                          13, 15));
            mRegions.push_back(new Region(REG_FOO,
                                          *mStrings[STR_FOO],
                                          *mStrings[STR_FOO],
                                          *mStrings[STR_EMPTY],
                                          Region::ROLE_FUNCTION,
                                          Paradigm::SAMPLING,
                                          *mStrings[STR_SOURCE],
                                          7, 10));
            mRegions.push_back(new Region(REG_BAR,
                                          *mStrings[STR_BAR],
                                          *mStrings[STR_BAR],
                                          *mStrings[STR_EMPTY],
                                          Region::ROLE_FUNCTION,
                                          Paradigm::SAMPLING,
                                          String::UNDEFINED,
                                          2, 4));

            mSourceLocs.push_back(new SourceLocation(SCL_MAIN,
                                                     *mStrings[STR_SOURCE],
                                                     14));
            mSourceLocs.push_back(new SourceLocation(SCL_FOO,
                                                     *mStrings[STR_SOURCE],
                                                     8));
            mSourceLocs.push_back(new SourceLocation(SCL_BAR,
                                                     *mStrings[STR_SOURCE],
                                                     3));

            mContextTree = new ContextTree;
            mContextTree->addContext(new CallingContext(CC_M,
                                                        *mRegions[REG_MAIN],
                                                        *mSourceLocs[SCL_MAIN],
                                                        0));
            mContextTree->addContext(new CallingContext(CC_M_F0,
                                                        *mRegions[REG_FOO],
                                                        *mSourceLocs[SCL_FOO],
                                                        mContextTree->getContext(CC_M)));
            mContextTree->addContext(new CallingContext(CC_M_F0_B,
                                                        *mRegions[REG_BAR],
                                                        *mSourceLocs[SCL_BAR],
                                                        mContextTree->getContext(CC_M_F0)));
            mContextTree->addContext(new CallingContext(CC_M_F1,
                                                        *mRegions[REG_FOO],
                                                        SourceLocation::UNDEFINED,
                                                        mContextTree->getContext(CC_M)));
            mContextTree->addContext(new CallingContext(CC_M_F1_B,
                                                        *mRegions[REG_BAR],
                                                        SourceLocation::UNDEFINED,
                                                        mContextTree->getContext(CC_M_F1)));
        }

        virtual void
        TearDown()
        {
            for_each(mStrings.begin(), mStrings.end(), delete_ptr<String>());
            for_each(mRegions.begin(), mRegions.end(), delete_ptr<Region>());
            for_each(mSourceLocs.begin(), mSourceLocs.end(),
                     delete_ptr<SourceLocation>());
            delete mContextTree;
        }

    protected:
        vector<String*>         mStrings;
        vector<Region*>         mRegions;
        vector<SourceLocation*> mSourceLocs;
        ContextTree*            mContextTree;
};


TEST_F(ContextTreeT, testAddContext)
{
    // Calling context with invalid (i.e., already taken) ID
    CallingContext item(CC_M_F1_B,
                        *mRegions[REG_BAR],
                        *mSourceLocs[SCL_BAR],
                        0);

    EXPECT_THROW(mContextTree->addContext(&item), RuntimeError);
}


TEST_F(ContextTreeT, testStreamOutput)
{
    string expected(" + main @ test.c:14 <0>\n"
                    "    + foo @ test.c:8 <1>\n"
                    "    |  + bar @ test.c:3 <2>\n"
                    "    + foo @ test.c <3>\n"
                    "       + bar <4>\n");

    ostringstream result;
    result << *mContextTree;
    EXPECT_EQ(expected, result.str());
}
