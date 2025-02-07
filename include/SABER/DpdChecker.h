//===- LeakChecker.h -- Detecting memory leaks--------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013->  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//

/*
 * LeakChecker.h
 *
 *  Created on: Apr 1, 2014
 *      Author: rockysui
 */

#ifndef DpdChecker_H_
#define DpdChecker_H_

#include "SABER/SrcSnkDDA.h"
#include "SABER/SaberCheckerAPI.h"

namespace SVF
{

/*!
 * Static Use After Free checker
 */
class DpdChecker : public SrcSnkDDA
{

public:
    typedef Map<const SVFGNode*,const CallICFGNode*> SVFGNodeToCSIDMap;
    typedef Map<const SVFGNode*,List<const CallICFGNode*>> SVFGNodeToCSIDsMap;
    typedef FIFOWorkList<const CallICFGNode*> CSWorkList;
    typedef FIFOWorkList<const SVFGNode*> NodeWorkList;
    typedef FIFOWorkList<const ICFGNode*> ICFGNodeWorkList;
    typedef ProgSlice::VFWorkList WorkList;
    typedef NodeBS SVFGNodeBS;
    typedef NodeBS ICFGNodeBS;
    enum LEAK_TYPE
    {
        NEVER_FREE_LEAK,
        CONTEXT_LEAK,
        PATH_LEAK,
        GLOBAL_LEAK
    };

    /// Constructor
    DpdChecker()
    {
    }
    /// Destructor
    virtual ~DpdChecker()
    {
    }

    /// We start from here
    virtual bool runOnModule(SVFModule* module)
    {
        /// start analysis
        analyze(module);
        return false;
    }

    /// Initialize sources and sinks
    //@{
    /// Initialize sources and sinks
    virtual void initSrcs() override;
    virtual void initSnks() override;
    /// Whether the function is a heap allocator/reallocator (allocate memory)
    virtual inline bool isSourceLikeFun(const SVFFunction* fun) override
    {
        return SaberCheckerAPI::getCheckerAPI()->isMemAlloc(fun);
    }
    /// Whether the function is a heap deallocator (free/release memory)
    virtual inline bool isSinkLikeFun(const SVFFunction* fun) override
    {
        return SaberCheckerAPI::getCheckerAPI()->isMemDealloc(fun);
    }
    //@}

    const std::string getSVFGNodeLoc(const SVFGNode* N);
protected:
    /// Report leaks
    //@{
    virtual void reportBug(ProgSlice* slice) override;
    void reportAlwaysUAF(ProgSlice* slice);
    void reportConditionalUAF(ProgSlice* slice);
    //@}
    /// Validate test cases for regression test purpose
    void testsValidation(const ProgSlice* slice);
    void validateSuccessTests(const SVFGNode* source, const SVFFunction* fun);
    void validateExpectedFailureTests(const SVFGNode* source, const SVFFunction* fun);

    /// Record a source to its callsite
    //@{
    inline void addSrcToCSID(const SVFGNode* src, const CallICFGNode* cs)
    {
        srcToCSIDMap[src] = cs;
    }
    inline const CallICFGNode* getSrcCSID(const SVFGNode* src)
    {
        SVFGNodeToCSIDMap::iterator it =srcToCSIDMap.find(src);
        assert(it!=srcToCSIDMap.end() && "source node not at a callsite??");
        return it->second;
    }

    /// Record a source to its callsites
    //@{
    inline void addSrcToCSIDs(const SVFGNode* src, const CallICFGNode* cs)
    {
        srcToCSIDsMap[src].push(cs);
    }
    inline List<const CallICFGNode*> getSrcCSIDs(const SVFGNode* src)
    {
        SVFGNodeToCSIDsMap::iterator it =srcToCSIDsMap.find(src);
        assert(it!=srcToCSIDsMap.end() && "source node not at a callsite??");
        return it->second;
    }
    //@}
private:
    SVFGNodeToCSIDMap srcToCSIDMap;
    SVFGNodeToCSIDsMap srcToCSIDsMap;

};

} // End namespace SVF

#endif /* DpdChecker_H_ */
