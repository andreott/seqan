// ==========================================================================
//                 SeqAn - The Library for Sequence Analysis
// ==========================================================================
// Copyright (c) 2006-2013, Knut Reinert, FU Berlin
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
// Author: Hannes Hauswedell <hauswedell@mi.fu-berlin.de>
// ==========================================================================
// Tests for seqan/stream/write_fasta_fastq.h
// ==========================================================================


#ifndef TEST_STREAM_TEST_STREAM_WRITE_FASTA_H_
#define TEST_STREAM_TEST_STREAM_WRITE_FASTA_H_

SEQAN_DEFINE_TEST(test_stream_write_record_fasta_default)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    CharString meta1 = "meta1";
    CharString meta2 = "  meta2 ";
    Dna5String seq1 = "CGATN";
    Dna5String seq2 = "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN";
    
    SEQAN_ASSERT_EQ(writeRecord(outStream, meta1, seq1, Fasta()), 0);
    SEQAN_ASSERT_EQ(writeRecord(outStream, meta2, seq2, Fasta()), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            ">meta1\n"
            "CGATN\n"
            ">  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTN\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

SEQAN_DEFINE_TEST(test_stream_write_record_fasta_nolinebreaks)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    CharString meta1 = "meta1";
    CharString meta2 = "  meta2 ";
    Dna5String seq1 = "CGATN";
    Dna5String seq2 = "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN";
    
    SEQAN_ASSERT_EQ(writeRecord(outStream, meta1, seq1, Fasta(), SequenceOutputOptions(0)), 0);
    SEQAN_ASSERT_EQ(writeRecord(outStream, meta2, seq2, Fasta(), SequenceOutputOptions(0)), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            ">meta1\n"
            "CGATN\n"
            ">  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTN\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

SEQAN_DEFINE_TEST(test_stream_write_record_fastq_default_separate_qual)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    CharString meta1 = "meta1";
    CharString meta2 = "  meta2 ";
    Dna5String seq1 = "CGATN";
    Dna5String seq2 = "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN";
    CharString qual1 = "!!!!!";
    CharString qual2 = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

    SEQAN_ASSERT_EQ(writeRecord(outStream, meta1, seq1, qual1, Fastq()), 0);
    SEQAN_ASSERT_EQ(writeRecord(outStream, meta2, seq2, qual2, Fastq()), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            "@meta1\n"
            "CGATN\n"
            "+\n"
            "!!!!!\n"
            "@  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "+\n"
            "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

SEQAN_DEFINE_TEST(test_stream_write_record_fastq_default_qual_in_seq)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    CharString meta1 = "meta1";
    CharString meta2 = "  meta2 ";
    String<Dna5Q> seq1 = "CGATN";
    assignQualities(seq1, CharString("!!!!!"));
    String<Dna5Q> seq2 = "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN";
    assignQualities(seq2, CharString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));

    SEQAN_ASSERT_EQ(writeRecord(outStream, meta1, seq1, Fastq()), 0);
    SEQAN_ASSERT_EQ(writeRecord(outStream, meta2, seq2, Fastq()), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            "@meta1\n"
            "CGATN\n"
            "+\n"
            "!!!!!\n"
            "@  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "+\n"
            "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

SEQAN_DEFINE_TEST(test_stream_write_record_fastq_linebreaks_qualmeta)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    CharString meta1 = "meta1";
    CharString meta2 = "  meta2 ";
    Dna5String seq1 = "CGATN";
    Dna5String seq2 = "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN";
    CharString qual1 = "!!!!!";
    CharString qual2 = "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

    SEQAN_ASSERT_EQ(writeRecord(outStream, meta1, seq1, qual1, Fastq(), SequenceOutputOptions(70, true)), 0);
    SEQAN_ASSERT_EQ(writeRecord(outStream, meta2, seq2, qual2, Fastq(), SequenceOutputOptions(70, true)), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            "@meta1\n"
            "CGATN\n"
            "+meta1\n"
            "!!!!!\n"
            "@  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "+  meta2 \n"
            "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
            "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

SEQAN_DEFINE_TEST(test_stream_write2_fasta_default)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    StringSet<CharString> metas;
    appendValue(metas, "meta1");
    appendValue(metas, "  meta2 ");
    StringSet<Dna5String> seqs;
    appendValue(seqs, "CGATN");
    appendValue(seqs, "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN");
    
    SEQAN_ASSERT_EQ(write2(outStream, metas, seqs, Fasta()), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            ">meta1\n"
            "CGATN\n"
            ">  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTN\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

SEQAN_DEFINE_TEST(test_stream_write2_fastq_default_qual_in_seq)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    StringSet<CharString> metas;
    appendValue(metas, "meta1");
    appendValue(metas, "  meta2 ");
    StringSet<String<Dna5Q> > seqs;
    appendValue(seqs, "CGATN");
    assignQualities(seqs[0], CharString("!!!!!"));
    appendValue(seqs, "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN");
    assignQualities(seqs[1], CharString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));

    SEQAN_ASSERT_EQ(write2(outStream, metas, seqs, Fastq()), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            "@meta1\n"
            "CGATN\n"
            "+\n"
            "!!!!!\n"
            "@  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "+\n"
            "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

SEQAN_DEFINE_TEST(test_stream_write2_fastq_default_separate_qual)
{
    using namespace seqan;

    char buffer[1000];
    Stream<CharArray<char *> > outStream(&buffer[0], &buffer[0] + 1000);

    StringSet<CharString> metas;
    appendValue(metas, "meta1");
    appendValue(metas, "  meta2 ");
    StringSet<Dna5String> seqs;
    appendValue(seqs, "CGATN");
    appendValue(seqs, "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN");
    StringSet<CharString> quals;
    appendValue(quals, "!!!!!");
    appendValue(quals, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

    SEQAN_ASSERT_EQ(write2(outStream, metas, seqs, quals, Fastq()), 0);
    streamPut(outStream, '\0');

    char const * EXPECTED =
            "@meta1\n"
            "CGATN\n"
            "+\n"
            "!!!!!\n"
            "@  meta2 \n"
            "CCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTNCCCAAATTTN\n"
            "+\n"
            "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    SEQAN_ASSERT_EQ(CharString(buffer), CharString(EXPECTED));
}

#endif // def TEST_STREAM_TEST_STREAM_WRITE_FASTA_H_
