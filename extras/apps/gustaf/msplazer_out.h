// ==========================================================================
//                                  Gustaf
// ==========================================================================
// Copyright (c) 2011-2013, Kathrin Trappe, FU Berlin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Knut Reinert or the FU Berlin nor the names of
//       its contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL KNUT REINERT OR THE FU BERLIN BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ==========================================================================
// Author: Kathrin Trappe <kathrin.trappe@fu-berlin.de>
// ==========================================================================

#ifndef SEQAN_EXTRAS_APPS_GUSTAF_MSPLAZER_OUT_H_
#define SEQAN_EXTRAS_APPS_GUSTAF_MSPLAZER_OUT_H_

#include <iostream>
#include <fstream>
#include <seqan/gff_io.h>
#include <seqan/vcf_io.h>
#include <seqan/file.h>

#include "../../../core/apps/stellar/stellar.h"
#include "msplazer.h"
#include "gustaf_matepairs.h"

using namespace seqan;

/**
.Tag.DotDrawingMSplazer
..cat:Input/Output
..summary:Switch to trigger drawing in dot format.
..value.DotDrawingMSplazer:Graphs in dot format.
..include:seqan/msplazer.h
*/

struct DotDrawingMSplazer_;
typedef Tag<DotDrawingMSplazer_> DotDrawingMSplazer;

/**
.Tag.DotDrawingMSplazerBestChain
..cat:Input/Output
..summary:Switch to trigger drawing in dot format.
..value.DotDrawingMSplazerBestChain:Best chain graphs in dot format.
..include:seqan/msplazer.h
*/

struct DotDrawingMSplazerBestChain_;
typedef Tag<DotDrawingMSplazer_> DotDrawingMSplazerBestchain;


/**
.Function.write:
..signature:write(file, msplazerchain, stellarmatches, tag)
..param.file:The file to write to.
...type:Class.Graph
..param.tag:A tag to select the output format.
...type:Tag.DotDrawingMSplazer
..include:seqan/msplazer.h
 */
template <typename TGraph, typename TVertexDescriptor, typename TScoreAlloc, typename TMatch, // typename TFile, 
          typename TBreakpoint, typename TPos, typename TMatchAlloc>
// typename TBreakpointAlloc, typename TMatchAlloc> // Requires Value<SparsePropertyMap> specialisation in msplazer.h
void
write(std::ostream & out, // TFile & file,  // std::ostream & out,  // std::fstream f; f.open(..); if (!f.good()) ... ; write(f, ...);  // write(std::cerr/std::cout, ...
      // MSplazerChain<TGraph, TVertexDescriptor, TScoreAlloc, TBreakpointAlloc, // Requires Value<SparsePropertyMap> specialisation in msplazer.h
      MSplazerChain<TGraph, TVertexDescriptor, TScoreAlloc, SparsePropertyMap<TBreakpoint, TPos>,
                    TMatchAlloc> const & msplazerchain,
      TMatch const & queryMatches,
      unsigned const & queryLength,
      DotDrawingMSplazer const &)
{
    // IOREV _doc_ _batchreading_
    SEQAN_CHECKPOINT
    // typedef typename Value<TBreakpointAlloc>::Type TBreakpoint; // Requires Value<SparsePropertyMap> specialisation in msplazer.h
    typedef typename TBreakpoint::TId TId;

    // _writeGraphType(file,g,DotDrawing());
    out << "digraph G {\n";
    out << '\n';
    out << "/* Graph Attributes */\n";
    out << "graph [rankdir = LR, clusterrank = local];\n";
    out << '\n';
    out << "/* Node Attributes */\n";
    out << "node [shape = rectangle, fillcolor = white, style = filled, fontname = \"Times-Italic\"];\n";
    out << '\n';
    out << "/* Edge Attributes */\n";
    out << "edge [fontname = \"Times-Italic\", arrowsize = 0.75, fontsize = 16];\n";
    out << '\n';

    out << "/* Nodes */\n";
    typedef typename Iterator<TGraph, VertexIterator>::Type TConstIter;
    TConstIter it(msplazerchain.graph);
    unsigned i = 0;
    bool atEndV = false;
    for (; !atEnd(it); ++it)
    {
        TId sId;
        if (i < length(queryMatches))
        {
            out << *it;
            out << " [label = \"";
            out << "chr: ";
            _getShortId(sId, queryMatches[i].id);
            out << sId;
            out << "\\n db: ";
            out << queryMatches[i].begin1 + 1;
            out << "...";
            out << queryMatches[i].end1;
            out << "  ";
            out << (queryMatches[i].orientation ? '+' : '-');
            out << "\\n read: ";
            out << queryMatches[i].begin2 + 1;
            out << "...";
            out << queryMatches[i].end2;
            out << "\\n";
            out << i;
            if (_isLeftMate(queryMatches[i], msplazerchain.mateJoinPosition))
                out << "\", fontcolor= green];\n";
            else
                out << "\", fontcolor= blue];\n";
        }
        else if (!atEndV)
        {
            out << *it;
            out << " [label = \"start";
            out << "\"];\n";
            atEndV = true;
        }
        else if (atEndV)
        {
            out << *it;
            out << " [label = \"end";
            out << "\\n";
            out << queryLength;
            out << "\"];\n";
        }
        else
            std::cerr << "in writing dot: default vertex??" << std::endl;
        ++i;
    }

    out << '\n';
    out << "/* Edges */\n";
    typedef typename Iterator<TGraph, EdgeIterator>::Type TConstEdIter;
    TConstEdIter itEd(msplazerchain.graph);
    for (; !atEnd(itEd); ++itEd)
    {
        TVertexDescriptor sc = sourceVertex(itEd);
        TVertexDescriptor tr = targetVertex(itEd);
        out << sc;
        _writeEdgeType(out, msplazerchain.graph, DotDrawing());
        out << tr;
        out << " [label = \"";
        out << getCargo(*itEd);
        TBreakpoint bp;
        bool foundBP = getProperty(msplazerchain.breakpoints, value(itEd), bp);
        if (foundBP)
            out << "*";
        out << "\"];\n";
    }
        out << '\n'
        << "}\n";
}

// DotWriting call for read graphs
template <typename TSequence, typename TId, typename TMSplazerChain>
void _writeDotfiles(StringSet<QueryMatches<StellarMatch<TSequence, TId> > > & stellarMatches,
                    StringSet<TSequence> const & queries,
                    String<TMSplazerChain> & queryChains,
                    MSplazerOptions const & msplazerOptions)
{
    for (unsigned i = 0; i < length(queryChains); ++i)
    {
        if (!queryChains[i].isEmpty)
        {
            std::string fn = "read" + toString(i + 1) + '_' + toString(msplazerOptions.jobName) + ".dot";
            std::ofstream f( toCString("read" + toString(i + 1) + '_' + toString(msplazerOptions.jobName) + ".dot"));
            if (!f.good())
                std::cerr << "Error while writing dot files!" << std::endl;
            write(f, queryChains[i], stellarMatches[i].matches, length(queries[i]), DotDrawingMSplazer());
            f.close();
        }
    }
}

template <typename TBreakpoint>
void _setGffRecordType(GffRecord & record, TBreakpoint & bp)
{
    switch(bp.svtype)
    {
        case TBreakpoint::INSERTION:
        record.type = "insertion";
        break;
        case TBreakpoint::DELETION:
        record.type = "deletion";
        break;
        case TBreakpoint::INVERSION:
        record.type = "inversion";
        break;
        case TBreakpoint::TANDEM:
        record.type = "tandem";
        break;
        case TBreakpoint::DISPDUPLICATION:
        record.type = "duplication";
        break;
        case TBreakpoint::INTRATRANSLOCATION:
        record.type = "intra-chr-translocation";
        break;
        case TBreakpoint::TRANSLOCATION:
        record.type = "translocation";
        break;
        case TBreakpoint::BREAKEND:
        record.type = "breakend";
        break;
        default:
        record.type = "invalid";
    }

}

template <typename TBreakpoint>
inline void _fillGffRecord(GffRecord & record, TBreakpoint & bp, unsigned id)
{
    typedef typename TBreakpoint::TId TId;
    TId sId;
    typedef typename TBreakpoint::TPos TPos;
    _getShortId(sId, bp.startSeqId);
    record.ref = sId;
    record.source = "GUSTAF";
    _setGffRecordType(record, bp);
//    record.type = bp.svtype;
    record.beginPos = bp.startSeqPos;
    if (bp.svtype == 2 || bp.svtype == 3) // 2=deletion;3=inversion
        record.endPos = bp.endSeqPos;
    else
        record.endPos = bp.startSeqPos + 1;
    if (bp.startSeqStrand)
        record.strand = '+';
    else
        record.strand = '-';

    appendValue(record.tagName, "ID");
    appendValue(record.tagValue, toString(id));
    if (bp.svtype == 1) // 1=insertion
    {
        appendValue(record.tagName, "size");
        appendValue(record.tagValue, '-' + toString(length(bp.insertionSeq)));
        appendValue(record.tagName, "seq");
        appendValue(record.tagValue, bp.insertionSeq);
    }
    else if (bp.svtype == 2) // 2=deletion
    {
        appendValue(record.tagName, "size");
        appendValue(record.tagValue, toString(static_cast<TPos>(bp.endSeqPos - bp.startSeqPos)));
    }
    else
    {
        appendValue(record.tagName, "endChr");
        _getShortId(sId, bp.endSeqId);
        appendValue(record.tagValue, sId);
        appendValue(record.tagName, "endPos");
        appendValue(record.tagValue, toString(bp.endSeqPos));
        appendValue(record.tagName, "endStrand");
        if (bp.endSeqStrand)
            appendValue(record.tagValue, '+');
        else
            appendValue(record.tagValue, '-');
    }
    appendValue(record.tagName, "support");
    appendValue(record.tagValue, toString(bp.support));
    appendValue(record.tagName, "supportIds");

    std::stringstream s;
    for (unsigned i = 0; i < length(bp.supportIds); ++i)
    {
        s << bp.supportIds[i];
        s << ',';
    }

    appendValue(record.tagValue, s.str());
    appendValue(record.tagName, "breakpoint");
    appendValue(record.tagValue, toString(bp.readStartPos + 1));
}

// Breakpoint writing call
template <typename TBreakpoint>
bool _writeGlobalBreakpoints(String<TBreakpoint> const & globalBreakpoints,
                             MSplazerOptions const & msplazerOptions, Gff /*tag*/)
{
    // Creating GFF record and writing to gff file
    std::string fn_gff = toString(msplazerOptions.gffOutFile);
    GffStream gffOut(fn_gff.c_str(), GffStream::WRITE);
    if (length(globalBreakpoints) == 0)
    {
        std::cerr << "Empty output list, skip writing gff" << std::endl;
        return 1;
    }

    if (!isGood(gffOut))
        std::cerr << "Error while opening gff breakpoint file!" << std::endl;

    GffRecord gff_record;
    for (unsigned i = 0; i < length(globalBreakpoints); ++i)
    {
        if (globalBreakpoints[i].svtype != 0 && globalBreakpoints[i].support >= msplazerOptions.support) // 0=invalid
        {
            // Fill record
            _fillGffRecord(gff_record, globalBreakpoints[i], i);
            // Write record
            if (writeRecord(gffOut, gff_record) != 0)
                std::cerr << "Error while writing breakpoint gff record!" << std::endl;
            clear(gff_record);
        }
    }

    std::cout << " completed gff writing " << msplazerOptions.gffOutFile << std::endl;
    return 0;
}

template <typename TBreakpoint, typename TSequence>
inline void _fillVcfRecordInsertion(VcfRecord & record, TBreakpoint & bp, TSequence & ref, unsigned id)
{
    record.rID = id;
    record.beginPos = bp.startSeqPos;
    record.filter = "PASS";
    std::stringstream ss;
    ss << "SVTYPE=INS";
    SEQAN_ASSERT_GEQ_MSG(bp.endSeqPos, bp.startSeqPos, "Insertion end position smaller than begin position!");
    ss << ";SVLEN=" << length(bp.insertionSeq);
    ss << ";DP=" << bp.support;
    record.info = ss.str();

    // Compute the number of bases in the REF column (1 in case of insertion and (k + 1) in the case of a
    // deletion of length k.
    if (bp.startSeqPos != 0)
        appendValue(record.ref, ref[bp.startSeqPos-1]); // position/base before SV except for position 1, then it is the base AFTER the event!
    else
        appendValue(record.ref, ref[bp.endSeqPos]); // position/base before SV except for position 1, then it is the base AFTER the event!

    // Compute ALT columns and a map to the ALT.
    appendValue(record.alt, record.ref[0]);
    if (length(bp.insertionSeq) < 20)
        append(record.alt, bp.insertionSeq);
    else
        append(record.alt, "<INS>");

    // Create genotype infos.
    appendValue(record.genotypeInfos, "1");
}

template <typename TBreakpoint, typename TSequence>
inline void _fillVcfRecordDeletion(VcfRecord & record, TBreakpoint & bp, TSequence & ref, unsigned id)
{
    record.rID = id;
    record.beginPos = bp.startSeqPos;
    record.filter = "PASS";
    std::stringstream ss;
    ss << "SVTYPE=DEL";
    SEQAN_ASSERT_GEQ_MSG(bp.endSeqPos, bp.startSeqPos, "Deletion end position smaller than begin position!");
    ss << ";SVLEN=-" << bp.endSeqPos-bp.startSeqPos;
    ss << ";DP=" << bp.support;
    record.info = ss.str();

    // Compute ALT columns and a map to the ALT.
    if (bp.startSeqPos != 0)
        appendValue(record.alt, ref[bp.startSeqPos-1]);// position/base before SV except for position 1, then it is the base AFTER the event!
    else
        appendValue(record.alt, ref[bp.endSeqPos]); // position/base before SV except for position 1, then it is the base AFTER the event!


    // Compute the number of bases in the REF column (1 in case of insertion and (k + 1) in the case of a
    // deletion of length k.
    appendValue(record.ref, record.alt[0]); // std::max(0,
    if ((bp.endSeqPos - bp.startSeqPos) < 20)
        append(record.ref, infix(ref, bp.startSeqPos, bp.endSeqPos)); // Deletions on reverse strand??? correct positions??
    else
        append(record.ref, "<DEL>");

    // Create genotype infos.
    appendValue(record.genotypeInfos, "1");
}

template <typename TBreakpoint, typename TSequence>
inline void _fillVcfRecordInversion(VcfRecord & record, TBreakpoint & bp, TSequence & ref, unsigned id)
{
    record.rID = id;
    record.beginPos = bp.startSeqPos;
    record.filter = "PASS";
    std::stringstream ss;
    ss << "SVTYPE=INV";
    ss << ";END=" << bp.endSeqPos;
    SEQAN_ASSERT_GEQ_MSG(bp.endSeqPos, bp.startSeqPos, "Inversion end position smaller than begin position!");
    ss << ";SVLEN=" << bp.endSeqPos-bp.startSeqPos;
    ss << ";DP=" << bp.support;
    record.info = ss.str();

    // Compute the number of bases in the REF column (1 in case of insertion and (k + 1) in the case of a
    // deletion of length k.
    if (bp.startSeqPos != 0)
        appendValue(record.ref, ref[bp.startSeqPos-1]); // std::max(0,
    else
        appendValue(record.ref, ref[bp.endSeqPos]); // std::max(0,

    // Compute ALT columns and a map to the ALT.
    append(record.alt, "<INV>");

    // Create genotype infos.
    appendValue(record.genotypeInfos, "1");
}

template <typename TBreakpoint, typename TSequence>
inline void _fillVcfRecordTandem(VcfRecord & record, TBreakpoint & bp, TSequence & ref, unsigned id)
{
    record.rID = id;
    record.beginPos = bp.startSeqPos;
    record.filter = "PASS";
    std::stringstream ss;
    ss << "SVTYPE=DUP";
    ss << ";END=" << bp.endSeqPos;
    SEQAN_ASSERT_GEQ_MSG(bp.endSeqPos, bp.startSeqPos, "Tandem duplication end position smaller than begin position!");
    ss << ";SVLEN=" << bp.endSeqPos-bp.startSeqPos;
    ss << ";DP=" << bp.support;
    record.info = ss.str();

    // Compute the number of bases in the REF column (1 in case of insertion and (k + 1) in the case of a
    // deletion of length k.
    if (bp.startSeqPos != 0)
        appendValue(record.ref, ref[bp.startSeqPos-1]); // std::max(0,
    else
        appendValue(record.ref, ref[bp.endSeqPos]); // std::max(0,

    // Compute ALT columns and a map to the ALT.
    append(record.alt, "<DUP:TANDEM>");

    // Create genotype infos.
    appendValue(record.genotypeInfos, "1");
}

template <typename TBreakpoint, typename TSequence>
inline void _fillVcfRecordDuplication(VcfRecord & record, TBreakpoint & bp, TSequence & ref, unsigned id)
{
    record.rID = id;
    record.beginPos = bp.startSeqPos;
    record.filter = "PASS";
    std::stringstream ss;
    ss << "SVTYPE=DUP";
    ss << ";END=" << bp.endSeqPos;
    SEQAN_ASSERT_GEQ_MSG(bp.endSeqPos, bp.startSeqPos, "Duplication end position smaller than begin position!");
    ss << ";SVLEN=" << bp.endSeqPos-bp.startSeqPos;
//    ss << ";TARGETPOS=" << bp.dupTargetPos;
    ss << ";DP=" << bp.support;
    record.info = ss.str();

    // Compute the number of bases in the REF column (1 in case of insertion and (k + 1) in the case of a
    // deletion of length k.
    if (bp.startSeqPos != 0)
        appendValue(record.ref, ref[bp.startSeqPos-1]); // std::max(0,
    else
        appendValue(record.ref, ref[bp.endSeqPos]); // std::max(0,

    // Compute ALT columns and a map to the ALT.
    append(record.alt, "<DUP>");

    // Create genotype infos.
    appendValue(record.genotypeInfos, "1");
}

template <typename TBreakpoint, typename TSequence>
inline void _fillVcfRecordBreakend(VcfRecord & record, TBreakpoint & bp, TSequence & ref, unsigned id)
{
    record.rID = id;
    record.beginPos = bp.startSeqPos;
    record.filter = "PASS";
    std::stringstream ss;
    // ss << "IMPRECISE;";
    ss << "SVTYPE=BND";
    // ss << ";CIPOS=-5,5";
    ss << ";DP=" << bp.support;
    record.info = ss.str();

    // Compute the number of bases in the REF column (1 in case of insertion and (k + 1) in the case of a
    // deletion of length k.
    if (bp.startSeqPos != 0)
        appendValue(record.ref, ref[bp.startSeqPos-1]); // std::max(0,
    else
        appendValue(record.ref, ref[bp.endSeqPos]); // std::max(0,

    // Compute ALT columns and a map to the ALT.
    // Storing if breakend is left of (0) or right of (1) breakpoint position
    if (bp.breakend)
    {
        appendValue(record.alt, record.ref[0]);
        appendValue(record.alt, '.');
    } else
    {
        appendValue(record.alt, '.');
        appendValue(record.alt, record.ref[0]);
    }

    // Create genotype infos.
    appendValue(record.genotypeInfos, "1");
}

template <typename TBreakpoint, typename TSequence>
inline bool _fillVcfRecord(VcfRecord & record, TBreakpoint & bp, TSequence & ref, unsigned id)
{
    switch(bp.svtype)
    {
        case TBreakpoint::INSERTION:
        _fillVcfRecordInsertion(record, bp, ref, id);
        return 1;
        case TBreakpoint::DELETION:
        _fillVcfRecordDeletion(record, bp, ref, id);
        return 1;
        case TBreakpoint::INVERSION:
        _fillVcfRecordInversion(record, bp, ref, id);
        return 1;
        case TBreakpoint::TANDEM:
        _fillVcfRecordTandem(record, bp, ref, id);
        return 1;
        case TBreakpoint::DISPDUPLICATION:
        _fillVcfRecordDuplication(record, bp, ref, id);
        return 1;
        // Translocation are handled seperately bc there is more than 1 record
        case TBreakpoint::INTRATRANSLOCATION:
        return 1;
        case TBreakpoint::TRANSLOCATION:
        return 1;
        case TBreakpoint::BREAKEND:
        _fillVcfRecordBreakend(record, bp, ref, id);
        return 1;

        default:
        return 0;
    }
    return 0;
}

template <typename TBreakpoint, typename TSequence>
inline bool _writeVcfTranslocation(VcfStream & vcfOut, TBreakpoint & bp, TSequence & ref, TSequence & ref2, unsigned id, unsigned id2, unsigned bp_id)
{
    // Translocation is encoded with 6 breakend entries (or 4 in the case where the "middle" position
    // of the translocation is not known)
    typedef typename TBreakpoint::TId TId;
    TId sId;
    VcfRecord record;
    // Record values shared by (almost) all entries
    record.rID = id;
    record.filter = "PASS";
    std::stringstream ss;
    ss << "SVTYPE=BND";
    ss << ";EVENT=Trans" << bp_id;
    ss << ";DP=" << bp.support;
    record.info = ss.str();
    // Create genotype infos.
    appendValue(record.genotypeInfos, "1");

    // 1st entry
    record.id = "BND_" + toString(bp_id) + "_1";
    record.beginPos = (bp.startSeqPos > 0) ? bp.startSeqPos - 1 : 0;

    // Positions encoded in REF and ALT coloumn have to be already 1-based
    // whereas record.beginPos remains 0-based and is adjusted within writeVcfRecord() function
    // Compute base for REF and ALT columns
    if (bp.startSeqPos != 0)
    {
        appendValue(record.ref, ref[bp.startSeqPos-1]);
        appendValue(record.alt, ref[bp.startSeqPos-1]);
    } else
    {
        appendValue(record.ref, 'N');
        appendValue(record.alt, '.');
    }

    // Compute ALT columns
    if (bp.dupMiddlePos != maxValue<unsigned>())
    {
        _getShortId(sId, bp.midPosId);
        std::stringstream alt1;
        alt1 << "[";
        alt1 << sId; // alt1 << bp.midPosId;
        alt1 << ':';
        alt1 << bp.dupMiddlePos + 1; // 1-based adjustment
        alt1 << "[";
        append(record.alt, alt1.str());
    } else
        appendValue(record.alt, '.');

    // Write record and clear REF and ALT values
    if (writeRecord(vcfOut, record) != 0)
    {
        std::cerr << "Error while writing breakpoint translocation entry 1 vcf record!" << std::endl;
        return 1;
    }
    clear(record.ref);
    clear(record.alt);

    // 2nd entry
    record.id = "BND_" + toString(bp_id) + "_2";
    record.beginPos = bp.startSeqPos;

    // Compute base for REF and ALT columns
    appendValue(record.ref, ref[bp.startSeqPos]);

    // Compute ALT columns
    if (bp.startSeqStrand != bp.endSeqStrand)
    {
        std::stringstream alt1;
        alt1 << "[";
        alt1 << bp.endSeqId;
        alt1 << ':';
        alt1 << bp.endSeqPos - 1 + 1; // 1-based adjustment
        alt1 << "[";
        append(record.alt, alt1.str());
    } else
    {
        std::stringstream alt1;
        alt1 << "]";
        alt1 << bp.endSeqId;
        alt1 << ':';
        alt1 << bp.endSeqPos - 1 + 1; // 1-based adjustment
        alt1 << "]";
        append(record.alt, alt1.str());
    }
    appendValue(record.alt, ref[bp.startSeqPos]);

    // Write record and clear REF and ALT values
    if (writeRecord(vcfOut, record) != 0)
    {
        std::cerr << "Error while writing breakpoint translocation entry 2 vcf record!" << std::endl;
        return 1;
    }
    clear(record.ref);
    clear(record.alt);

    // 3rd and 4th entry only exist if the middle (bp.dupMiddlePos) of translocation is known
    if (bp.dupMiddlePos != maxValue<unsigned>())
    {
        // 3rd entry
        record.id = "BND_" + toString(bp_id) + "_3";
        record.beginPos = bp.dupMiddlePos;

        // Compute base for REF and ALT columns
        appendValue(record.ref, ref[bp.dupMiddlePos]);
        appendValue(record.alt, ref[bp.dupMiddlePos]);

        // Compute ALT columns
        if (bp.startSeqStrand != bp.endSeqStrand)
        {
            std::stringstream alt1;
            alt1 << "]";
            alt1 << bp.endSeqId;
            alt1 << ':';
            alt1 << bp.endSeqPos + 1; // 1-based adjustment
            alt1 << "]";
            append(record.alt, alt1.str());
        } else
        {
            std::stringstream alt1;
            alt1 << "[";
            alt1 << bp.endSeqId;
            alt1 << ':';
            alt1 << bp.endSeqPos + 1; // 1-based adjustment
            alt1 << "[";
            append(record.alt, alt1.str());
        }

        // Write record and clear REF and ALT values
        if (writeRecord(vcfOut, record) != 0)
        {
            std::cerr << "Error while writing breakpoint translocation entry 3 vcf record!" << std::endl;
            return 1;
        }
        clear(record.ref);
        clear(record.alt);

        // 4th entry
        record.id = "BND_" + toString(bp_id) + "_4";
        record.beginPos = bp.dupMiddlePos + 1;

        // Compute ALT columns
        std::stringstream alt4;
        alt4 << "]";
        alt4 << sId; // alt1 << bp.midPosId;
        alt4 << ':';
        if (bp.startSeqPos > 0)
            alt4 << bp.startSeqPos - 1 + 1; // 1-based adjustment
        else 
            alt4 << 0; // NO(!) 1-based adjustment
        alt4 << "]";
        append(record.alt, alt4.str());

        // Compute base for REF and ALT columns
        appendValue(record.ref, ref[bp.dupMiddlePos+1]);
        appendValue(record.alt, ref[bp.dupMiddlePos+1]);

        // Write record and clear REF and ALT values
        if (writeRecord(vcfOut, record) != 0)
        {
            std::cerr << "Error while writing breakpoint translocation entry 4 vcf record!" << std::endl;
            return 1;
        }
        clear(record.ref);
        clear(record.alt);

    }

    record.rID = id2;
    // 5th entry
    record.id = "BND_" + toString(bp_id) + "_5";
    record.beginPos = bp.endSeqPos - 1;

    // Compute base for REF and ALT columns
    appendValue(record.ref, ref2[bp.endSeqPos - 1]);
    appendValue(record.alt, ref2[bp.endSeqPos - 1]);

    // Compute ALT columns
    if (bp.startSeqStrand != bp.endSeqStrand)
    {
        std::stringstream alt1;
        alt1 << "]";
        alt1 << bp.startSeqId;
        alt1 << ':';
        alt1 << bp.startSeqPos + 1; // 1-based adjustment
        alt1 << "]";
        append(record.alt, alt1.str());
    } else
    {
        std::stringstream alt1;
        alt1 << "[";
        alt1 << bp.startSeqId;
        alt1 << ':';
        alt1 << bp.startSeqPos + 1; // 1-based adjustment
        alt1 << "[";
        append(record.alt, alt1.str());
    }

    // Write record and clear REF and ALT values
    if (writeRecord(vcfOut, record) != 0)
    {
        std::cerr << "Error while writing breakpoint translocation entry 5 vcf record!" << std::endl;
        return 1;
    }
    clear(record.ref);
    clear(record.alt);

    // 6th entry
    record.id = "BND_" + toString(bp_id) + "_6";
    record.beginPos = bp.endSeqPos;

    // Compute ALT columns
    if (bp.dupMiddlePos != maxValue<unsigned>())
    {
        std::stringstream alt1;
        if (bp.endSeqStrand != bp.midPosStrand)
            alt1 << "[";
        else
            alt1 << "]";
        alt1 << sId; // alt1 << bp.midPosId;
        alt1 << ':';
        alt1 << bp.dupMiddlePos -1 + 1; // 1-based adjustment
        if (bp.endSeqStrand != bp.midPosStrand)
            alt1 << "[";
        else
            alt1 << "]";
        append(record.alt, alt1.str());
    } else
        appendValue(record.alt, '.');

    // Compute base for REF and ALT columns
    if (bp.endSeqPos < length(ref2) - 1)
    {
        appendValue(record.ref, ref2[bp.endSeqPos]);
        appendValue(record.alt, ref2[bp.endSeqPos]);
    } else
    {
        appendValue(record.ref, 'N');
        appendValue(record.alt, '.');
    }

    // Write record and clear REF and ALT values
    if (writeRecord(vcfOut, record) != 0)
    {
        std::cerr << "Error while writing breakpoint translocation entry 6 vcf record!" << std::endl;
        return 1;
    }
    clear(record.ref);
    clear(record.alt);

    return 0;
}

template <typename TSequence, typename TId>
void _fillVcfHeader(VcfStream & vcfStream, StringSet<TSequence> & databases, StringSet<TId> & databaseIDs, MSplazerOptions const & msplOpt)
{
    // Header record entries
    appendValue(vcfStream.header.headerRecords, VcfHeaderRecord("fileformat", "VCFv4.1"));
    appendValue(vcfStream.header.headerRecords, VcfHeaderRecord("source", "GUSTAF"));
    appendValue(vcfStream.header.headerRecords, VcfHeaderRecord("reference", msplOpt.databaseFile));
    for (unsigned i = 0; i < length(msplOpt.queryFile); ++i)
        appendValue(vcfStream.header.headerRecords, VcfHeaderRecord("reads", msplOpt.queryFile[i]));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
            "INFO", "<ID=END,Number=1,Type=Integer,Description=\"End position of the variant described in this record\">"));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
            "INFO", "<ID=SVLEN,Number=.,Type=Integer,Description=\"Difference in length between REF and ALT alleles\">"));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
            "INFO", "<ID=SVTYPE,Number=1,Type=String,Description=\"Type of structural variant\">"));
    // appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
    //            "INFO", "<ID=BND,Description=\"Breakend\">"));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
            "INFO", "<ID=EVENT,Number=1,Type=String,Description=\"Event identifier for breakends.\">"));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
            "INFO", "<ID=TARGETPOS,Number=1,Type=String,Description=\"Target position for duplications.\">"));
    appendValue(vcfStream.header.headerRecords, VcfHeaderRecord(
            "INFO", "<ID=DP,Number=1,Type=Integer,Description=\"Number of Supporting Reads/Read Depth for Variant\">"));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
            "ALT", "<ID=INV,Description=\"Inversion\">"));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
                "ALT", "<ID=DUP,Description=\"Duplication\">"));
    appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord(
                "ALT", "<ID=DUP:TANDEM,Description=\"Tandem Duplication\">"));

    // Fill sequence names
    for (unsigned i = 0; i < length(databaseIDs); ++i)
    {
        appendValue(vcfStream.header.sequenceNames, databaseIDs[i]);
        seqan::CharString contigStr = "<ID=";
        append(contigStr, databaseIDs[i]);
        append(contigStr, ",length=");
        std::stringstream ss;
        ss << length(databases[i]);
        append(contigStr, ss.str());
        append(contigStr, ">");
        appendValue(vcfStream.header.headerRecords, seqan::VcfHeaderRecord("contig", contigStr));
        appendName(*vcfStream._context.sequenceNames,
                   databaseIDs[i],
                   vcfStream._context.sequenceNamesCache);
    }
}

template <typename TId>
__int32 _getrID(StringSet<TId> & databaseIDs, TId dbID)
{
    for (unsigned i = 0; i < length(databaseIDs); ++i)
    {
        if (databaseIDs[i] == dbID)
            return static_cast<__int32>(i);
    }
    return maxValue<int>();
}

// Breakpoint writing call
template <typename TBreakpoint, typename TSequence, typename TId>
bool _writeGlobalBreakpoints(String<TBreakpoint> const & globalBreakpoints,
                             StringSet<TSequence> & databases,
                             StringSet<TId> & databaseIDs,
                             MSplazerOptions const & msplazerOptions,
                             Vcf /*tag*/)
{
    // Creating GFF record and writing to gff file
    std::string fn_vcf = toString(msplazerOptions.vcfOutFile);
    VcfStream vcfOut(fn_vcf.c_str(), VcfStream::WRITE);
    if (length(globalBreakpoints) == 0)
    {
        std::cerr << "Empty output list, skip writing vcf" << std::endl;
        return 1;
    }

    if (!isGood(vcfOut))
        std::cerr << "Error while opening vcf breakpoint file!" << std::endl;
    _fillVcfHeader(vcfOut, databases, databaseIDs, msplazerOptions);

    VcfRecord vcf_record;
    __int32 id = maxValue<int>();
    for (unsigned i = 0; i < length(globalBreakpoints); ++i)
    {
        TBreakpoint bp = globalBreakpoints[i];
        if (bp.svtype != 0 && bp.support >= msplazerOptions.support) // 0=invalid
        {
            id = _getrID(databaseIDs, bp.startSeqId);
            if (bp.svtype != 6 && bp.svtype != 7) // 6=intra-chr-translocation; 7=translocation
            {
                // Fill and write record
                if (_fillVcfRecord(vcf_record, bp, databases[id], id))
                    if (writeRecord(vcfOut, vcf_record) != 0)
                        std::cerr << "Error while writing breakpoint vcf record!" << std::endl;
                clear(vcf_record);
            } else
            {
                // extra write function because we have to write 6 records here instead of 1
                __int32 id2 = maxValue<int>();
                id2 = _getrID(databaseIDs, bp.endSeqId);
                if (_writeVcfTranslocation(vcfOut, bp, databases[id], databases[id2], id, id2, i))
                        std::cerr << "Error while writing breakpoint translocation vcf record!" << std::endl;

            }
        }
    }
    std::cout << " completed vcf writing " << msplazerOptions.vcfOutFile << std::endl;
    return 0;
}


// /////////////////////////////////////////////////////////////////////////////
// Writes parameters from options object to std::cout
template <typename TOptions>
void
_writeParams(TOptions & options)
{
// IOREV _notio_
    // Output user specified parameters
    // if (options.outDir != "")
    //  std::cout << "Output directory        : " << options.outDir << std::endl;
    if (options.jobName != "")
        std::cout << "Job name        : " << options.jobName << std::endl;
    std::cout << "Thresholds:" << std::endl;
    std::cout << "  overlap threshold (oth)          : " << options.simThresh << std::endl;
    std::cout << "  gap threshold (gth)              : " << options.gapThresh << std::endl;
    std::cout << "  inital gap threshold (ith)       : " << options.initGapThresh << std::endl;
    std::cout << "Penalties:" << std::endl;
    std::cout << "  translocation penalty (tp)       : " << options.diffDBPen << std::endl;
    std::cout << "  inversion penalty (ip)           : " << options.diffStrandPen << std::endl;
    std::cout << "  order penalty (op)               : " << options.diffOrderPen << std::endl;

    std::cout << "  required read support (st)       : " << options.support << std::endl;
}

/**TODO(ktrappe): Add write functionality for SAM/VCF format
.Function.write:
..signature:write(file, msplazerchain, stellarmatches, nodeMap, edgeMap, tag)
..param.graph:The graph to write out.
...type:Class.Graph
..param.nodeMap:A mapping from vertex descriptor to vertex label.
..param.edgeMap:A mapping from edge descriptor to edge label.
..param.tag:A tag to select the output format.
...type:Tag.SamMSplazer
..include:seqan/msplazer.h
 */

// ////////////////////
// SAM Output
// TODO(ktrappe): Use SamBamIO, see tutorial
template <typename TSequence, typename TId>
void _writeSamFile(CharString outfile,
                   StringSet<QueryMatches<StellarMatch<TSequence, TId> > > & stellarMatches,
                   StringSet<TSequence> & databases,
                   StringSet<CharString> databaseIDs,
                   StringSet<CharString> queryIDs)
{

    // CharString samOutFile2 = "msplazerStellarOut.sam";
    std::ofstream file2;
    file2.open(toCString(outfile), ::std::ios_base::out | ::std::ios_base::app);
    if (!file2.is_open())
    {
        std::cerr << "Could not open output file " << outfile << std::endl;
    }
    else
    {

        typedef StellarMatch<TSequence, TId> TMatch;
        typedef typename Iterator<String<TMatch> >::Type TIterator;
        typedef typename Row<typename TMatch::TAlign>::Type TMatchRow;
        typedef typename Size<TMatchRow>::Type TRowSize;
        TRowSize matchNumber, alignLen, alignDistance;

        //  std::cerr << "length: " << length(stellarMatches[0].matches);


        file2 << "@HD\tVN:1.0\tSO:coordinate\n";
        for (unsigned i = 0; i < length(databaseIDs); ++i)
            file2 << "@SQ\tSN:" << databaseIDs[i] << "\tLN:" << length(databases[i]) << "\n";
        // TODO(ktrappe): edit and flag CIGAR with clipped pos, mind strang direction
        for (unsigned i = 0; i < length(stellarMatches); ++i)
        {

            TIterator itStellarMatches = begin(stellarMatches[i].matches);
            TIterator itEndStellarMatches = end(stellarMatches[i].matches);

            if (itStellarMatches != itEndStellarMatches)
            {
                stellarMatches[i].lengthAdjustment = _computeLengthAdjustment(length(source((*itStellarMatches).row1)),
                                                                              length(source((*itStellarMatches).row2)));
            }

            //  std::cerr << "test itStellarMatches: " << *itStellarMatches
            // << " itEndStellarMatches: " << *itEndStellarMatches << std::endl;
            while (itStellarMatches < itEndStellarMatches)
            {

                // for(unsigned stellarmatches = 0; stellarmatches < length(matches[i].matches); ++stellarmatches){
                if ((*itStellarMatches).orientation)
                {
                    std::stringstream cigar, mutations;
                    _getCigarLine((*itStellarMatches).row1, (*itStellarMatches).row2, cigar, mutations);
                    //                                std::cout << "cigar string: " << cigar.str() << std::endl;
//                                    std::cout << "begin1: " << (*itStellarMatches).begin1 << std::endl;

                    // Alignment Score using _analyzeAlignment(row0, row1, alignLen, matchNumber) from stellar_output.h
                    // matchNumber and alignLen reset within _analyzeAlignment
                    _analyzeAlignment((*itStellarMatches).row1, (*itStellarMatches).row2, alignLen, matchNumber);
                    alignDistance = alignLen - matchNumber;
                    file2 << queryIDs[i] << "\t"
                          << "0\t"
                    // << databaseIDs[0] << "\t" //<< matches[i].matches[stellarmatches].id << "\t"
                          << (*itStellarMatches).id << "\t"
                    // 1-based position in DB/ref!
                          << "begin2: " << (*itStellarMatches).begin2 + 1 << "\t"
                          << "end2: " << (*itStellarMatches).end2 + 1 << "\t"
                          << "begin1: " << (*itStellarMatches).begin1 + 1 << "\t"
                          << "end1: " << (*itStellarMatches).end1 + 1 << "\t"
                          << ((*itStellarMatches).orientation ? '+' : '-') << "\t"
                          << "0\t"
                          << cigar.str() << "\t"
                          << "distance: " << alignDistance << "\t"
                          << "# matches: " << matchNumber << "\t"
                          << "alignLen: " << alignLen << "\t"
                    // << "*\t0\t0\t"
                    // << "MatchSeq" << "\t"
                    << "*\n";
                }
                ++itStellarMatches;                // better goNext()?
            }

            reverseComplement(databases);
            itStellarMatches = begin(stellarMatches[i].matches);

            while (itStellarMatches < itEndStellarMatches)
            {
                // for(unsigned stellarmatches = 0; stellarmatches < length(matches[i].matches); ++stellarmatches){

                if (!(*itStellarMatches).orientation)
                {

                    std::stringstream cigar, mutations;
                    _getCigarLine((*itStellarMatches).row1, (*itStellarMatches).row2, cigar, mutations);
                    //                                std::cout << "cigar string: " << cigar.str() << std::endl;
//                                    std::cout << "begin1: " << (*itStellarMatches).begin1 << std::endl;

                    // Alignment Score using _analyzeAlignment(row0, row1, alignLen, matchNumber) from stellar_output.h
                    // matchNumber and alignLen reset within _analyzeAlignment
                    _analyzeAlignment((*itStellarMatches).row1, (*itStellarMatches).row2, alignLen, matchNumber);
                    alignDistance = alignLen - matchNumber;
                    file2 << queryIDs[i] << "\t"
                          << "0\t"
                          << databaseIDs[0] << "\t"                   // << matches[i].matches[stellarmatches].id << "\t"
                          << (*itStellarMatches).id << "\t"
                          << "begin2: " << (*itStellarMatches).begin2 + 1 << "\t"                   // << matches[i].matches[stellarmatches].begin2 << "\t"
                          << "end2: " << (*itStellarMatches).end2 + 1 << "\t"                   // << matches[i].matches[stellarmatches].begin2 << "\t"
                          << "begin1: " << (*itStellarMatches).begin1 + 1 << "\t"                   // << matches[i].matches[stellarmatches].begin2 << "\t"
                          << "end1: " << (*itStellarMatches).end1 + 1 << "\t"
                          << ((*itStellarMatches).orientation ? '+' : '-') << "\t"
                          << "0\t"
                          << cigar.str() << "\t"
                          << "distance: " << alignDistance << "\t"
                          << "# matches: " << matchNumber << "\t"
                          << "alignLen: " << alignLen << "\t"
                          << "*\n";
                }
                ++itStellarMatches;                // better goNext()?
            }
            reverseComplement(databases);
            file2 << "\n";
        }
        file2.close();
    }
}

#endif  // #ifndef SANDBOX_MY_SANDBOX_APPS_MSPLAZER_MSPLAZER_OUT_H_
