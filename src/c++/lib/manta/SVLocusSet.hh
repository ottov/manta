// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Manta
// Copyright (c) 2013 Illumina, Inc.
//
// This software is provided under the terms and conditions of the
// Illumina Open Source Software License 1.
//
// You should have received a copy of the Illumina Open Source
// Software License 1 along with this program. If not, see
// <https://github.com/downloads/sequencing/licenses/>.
//

///
/// \author Chris Saunders
///

#pragma once

#include "manta/SVLocus.hh"

#include <iosfwd>
#include <map>
#include <vector>


// A set of non-overlapping SVLocus objects
//
struct SVLocusSet
{
    typedef std::vector<SVLocus> locusset_type;
    typedef locusset_type::const_iterator const_iterator;

    unsigned
    size() const
    {
        return _loci.size();
    }

    const_iterator
    begin() const
    {
        return _loci.begin();
    }

    const_iterator
    end() const
    {
        return _loci.end();
    }

    /// merge new locus into the set:
    ///
    /// locus is destroyed in this process
    ///
    void
    merge(SVLocus& locus);

    void
    clear()
    {
        _loci.clear();
        _emptyLoci.clear();
        _inodes.clear();
    }

    // binary serialization
    void
    save(const char* filename) const;

    // restore from binary serialization
    void
    load(const char* filename);

    // debug output
    void
    dump(std::ostream& os) const;

#if 0
    // debug output
    void
    dump_region(std::ostream& os,
            const int32_t tid,
            const int32_t beginPos,
            const int32_t endPos) const;
#endif

private:
    typedef SVLocusNode* ins_key_type;

    struct insKeySorter {
        bool
        operator()(
            const ins_key_type& a,
            const ins_key_type& b) const
        {
            if((a->interval)<(b->interval)) return true;
            if((a->interval)==(b->interval))
            {
                return (a<b);
            }
            return false;
        }
    };

    typedef std::map<ins_key_type, unsigned, insKeySorter> ins_type;


    unsigned
    getStartLocusIndex() const;


    /// get all nodes in this object which intersect with
    /// the inputNode
    void
    getNodeIntersect(
        SVLocusNode* inputNodePtr,
        ins_type& intersect);

    /// combine all content from loci from into to
    ///
    /// this is typically required when a node is merged
    /// which combines two loci
    void
    combineLoci(
        const unsigned fromIndex,
        const unsigned toIndex);

    // add node from a locus which is not part of this locusSet
    void
    insertLocusNode(
        const unsigned locusIndex,
        SVLocus& inputLocus,
        SVLocusNode* inputNodePtr)
    {
        assert(NULL != inputNodePtr);
        assert(locusIndex < _loci.size());

        _loci[locusIndex].copyNode(inputLocus,inputNodePtr);
        _inodes[inputNodePtr] = locusIndex;
    }

    void
    removeNode(SVLocusNode* inputNodePtr)
    {
        assert(NULL != inputNodePtr);

        ins_type::iterator iter(_inodes.find(inputNodePtr));
        if (iter == _inodes.end()) return;

        const unsigned index(iter->second);

        assert(index<_loci.size());

        SVLocus& locus(_loci[index]);
        locus.erase(inputNodePtr);
        _inodes.erase(iter);
    }

    void
    mergeNodePtr(SVLocusNode* fromPtr,
                 SVLocusNode* toPtr);

    void
    reconstructIndex();

    void
    dumpIndex(std::ostream& os) const;


    /// check that internal data-structures are in
    /// a consistent state, throw on error
    void
    checkState() const;

    ///////////////////// data

    // contains the full set of loci
    locusset_type _loci;
    std::set<unsigned> _emptyLoci;

    // provides an intersection search of non-overlapping nodes:
    ins_type _inodes;
};
